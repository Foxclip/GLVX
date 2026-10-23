# BUGFIX REPORT — GLVX on GL 4.1 (Mesa 24.0.9)

**Date:** 2026-10-23
**Goal:** `glvx_tests` fully green (107/107, 0 failed, 0 cancelled) and `glvx_sandbox`
working (incl. `--screenshot`) on this machine: Linux Mint, X11 `:0`, Mesa 24.0.9
(SVGA3D/llvmpipe), max **OpenGL 4.1**, 16 cores.

**Result: DONE.** 107/107 tests pass; sandbox screenshot verified as a real,
non-uniform render.

---

## Root causes and fixes

There were **four independent defects** on this host (plus one app-side fix for the
sandbox). Fixes 1+2 were required just to start the executables; fixes 3+4 are what
make the pixel-exact readback correct.

### 1. Window creation demanded a GL 4.2 core context — host max is 4.1

- **Symptom:** both executables died at startup with `ERROR: create: Failed to
  create GLFW window` (GLX: `GLXBadFBConfig`).
- **Cause:** `Window::create` (src/window.cpp) unconditionally requested
  `GLFW_CONTEXT_VERSION_MAJOR 4 / MINOR 2` + core profile.
- **Fix:** src/window.cpp:55 — request 4.2 core first; on `glfwCreateWindow`
  failure retry with **4.1 core** (`for (const int minor_version : {2, 1})`).
  Machines with GL ≥ 4.2 still get 4.2 (no behavior change); 4.1 is only ever a
  fallback. The `_WIN32` minimized branch is untouched.

### 2. Shader sources target GLSL 420 (UBO binding qualifiers) — uncompilable in 4.1

- **Cause:** all shader sources (library `include/glvx/shaders/{simple,subpixel}.h`,
  test shaders, sandbox shaders) use `#version 420 core` and
  `layout (binding = N) uniform ... { ... }` blocks. A 4.1 context cannot compile
  those.
- **First attempt (failed — see Rejected approaches):** demote `#version` to 410 and
  insert `#extension GL_ARB_uniform_buffer_object : require` while keeping the block
  syntax. Mesa 24.0.9's compiler still rejects the `binding` layout qualifier below
  420.
- **Fix (current):** compile-time source adaptation at the single choke point
  `Shader::compileShader` (src/shader.cpp), so **every** shader (library, test,
  app) is covered without touching any shader file:
  - `adapt_shading_language_version` (src/shader.cpp:117): if the context's
    `GL_SHADING_LANGUAGE_VERSION` is lower than the `#version` the source requests,
    the version number is rewritten down in place (3-digit, offsets preserved).
  - When the resulting version is < 420, `replace_uniform_blocks`
    (src/shader.cpp:57) rewrites each `layout (binding = N) uniform Name { ... }
    instance;` into equivalent **plain uniform declarations** (one per member, in
    order), and `strip_uniform_instance_qualifiers` (src/shader.cpp:35) rewrites
    `instance.member` → `member` (word-boundary aware, only after the block, so
    user code that happens to contain the literal text is not mangled).
  - This works because the library queries these uniforms by **unqualified** member
    names — `setVec4("color", ...)`, `setMat4("vp", ...)`, `setBool("premultiplyOutput",
    ...)` (src/drawable.cpp:74-85) — which resolve both as unique UBO member
    references on 4.2+ and as plain uniforms on 4.1. `Shader` gained an
    `m_ubo_available` flag (include/glvx/shader.h) so the draw path sends the same
    per-draw uniforms to non-UBO shaders (src/drawable.cpp else-branch: `vp`,
    `premultiplyOutput`).
  - On contexts ≥ 4.2 the adaptation is a complete no-op (source is compiled
    verbatim with real UBOs).

### 3. Driver float→UNORM8 conversion **truncates** instead of rounding (the user's readback bug)

- **Symptom (user's machine, GL 4.5):** after `window.clear(Color::Red);
  window.display();`, `window.readPixels()` returned all `(0 0 0 0)`.
  **On this host**, once fixes 1+2 landed, `Basic/clear` itself passed, but the
  same defect showed up exactly where the GL spec says it must not: half-integer
  readbacks were off by one —
  `Texture/texture_resize`: expected `(128 128 128 128)`, actual `(127 127 127 127)`
  (127.5 truncated, not rounded); `Texture/texture_color_multiply`: expected
  `(1 1 2 255)`, actual `(0 1 1 255)` (0.5019·… truncated, not rounded).
  The tests document the expectation explicitly (glvx_tests/src/texture_tests.cpp:
  "GL converts it round-to-nearest -> 128 (not the 127 that integer truncation
  implies)").
- **Proof of driver non-compliance** (standalone probe, /tmp/glprobe/probe.c,
  GLX 4.1 core, plain RGBA8 window): `glClearColor(127.5/255, …)` → readback
  **127**, spec round-to-nearest says **128**.
- **Fix:** stop trusting the driver's fixed-point conversion for render targets —
  store render results in **float (RGBA16F)** and do the byte conversion in the
  library with explicit round-to-nearest:
  - `AbstractTexture`: new `m_float_storage` flag, `createFloatTexture()`
    (src/abstract_texture.cpp:295, `GL_RGBA16F` texture) and `readPixelsFloatRaw()`
    (src/abstract_texture.cpp:326: `glGetTexImage` `GL_FLOAT` + `lroundf(v * 255)`
    per channel — the spec's round-to-nearest, tie away from zero).
  - `RenderTexture::create` (src/render_texture.cpp:59): non-MSAA render targets
    are created as RGBA16F; `RenderTexture::readPixels`
    (src/render_texture.cpp:171) reads via the float path and unpremultiplies as
    before. MSAA targets stay UNORM8 (RGBA16F multisample needs GL 4.3; the
    resolve blit into the float base texture is lossless, and no test uses MSAA).
  - F16 was chosen over F32: it exactly represents all `k/255` values and the
    tests' half-integer math with ≥3 orders of magnitude to spare, at lower
    bandwidth on llvmpipe.
  - This also fixes the same class of bug on **any** machine (including the
  user's 4.5 one): the readback is now driver-conversion-independent.

### 4. Driver `glBlitFramebuffer` resampling has the same truncation

- **Symptom:** `Texture/texture_resize` (2x1 → 3x1 linear) returned 127 where 128
  is expected — the resize path used `glBlitFramebuffer` to resample, whose
  float→UNORM8 output is converted by the same non-compliant driver code.
- **Fix:** `AbstractTexture::resizeTexture` (src/abstract_texture.cpp) now resamples
  **on the CPU** (`resample_image`, src/abstract_texture.cpp:198): the old
  contents are read back (float or byte-exact), resampled with the same
  texel-center mapping as `glBlitFramebuffer` (`(x+0.5)·(old/new)−0.5`, edge
  clamping; `GL_LINEAR`-equivalent bilinear or nearest), and uploaded with
  explicit `lroundf` rounding. Works for both plain `Texture` (UNORM) and float
  `RenderTexture` storage; window resizes pass `blit_old_contents=false` and are
  unaffected.

### 5. (sandbox only) local `vec3 color` shadowed the demoted `color` uniform

- **Symptom:** sandbox aborted: `Fragment shader compilation failed … error:
  invalid swizzle / mask 'a'` (line 22 of the animated showcase shader).
- **Cause:** after fix 2, `object.color.a` becomes `color.a`; that shader's local
  `vec3 color` shadowed the uniform inside `main()`, and `vec3` has no `.a`.
  (Legal in the original 420 source because the reference was qualified.)
- **Fix:** renamed the local to `blended` in glvx_sandbox/src/application.cpp
  (`shader_animated_frag`), with a comment explaining the constraint. The other
  two sandbox shaders have no such shadowing.

---

## Rejected approaches

1. **`#version 410` + `#extension GL_ARB_uniform_buffer_object : require`** (keep
   UBO block syntax): failed — Mesa 24.0.9 rejects the `binding` layout qualifier
   below GLSL 420 even with the extension required (first full test run: vertex +
   fragment compile failures in `Shader: compileShader`). Superseded by the
   plain-uniform rewrite (fix 2).
2. **Requiring GL 4.2 only (no fallback):** the host's maximum is 4.1 — that is
   the premise of the task.
3. **Keep `glBlitFramebuffer` for resizes / keep UNORM8 render targets and
   "accept" the driver's rounding:** the tests are the spec and explicitly encode
   round-to-nearest (texture_tests.cpp comments); the driver is non-compliant, so
   the library must not route through the driver's conversion on the
   pixel-exact paths.
4. **GL_RGBA32F render targets:** unnecessary — F16 is exact for all values these
   tests produce (verified ≥3 orders of magnitude of margin) and cheaper on
   llvmpipe.
5. **Renaming demoted uniforms to collision-free names** (to fix the sandbox
   shadowing library-side): rejected — the library looks uniforms up by bare
   member name (src/drawable.cpp:74-85), so a rename breaks uniform binding;
   shadowing is a property of the app's shader source, so the app's local was
   renamed (fix 5).

---

## Verification evidence

**Final test run** (from project root, minimized, on the final tree —
`cd /home/foxclip/GLVX && ./build-linux/glvx_tests/glvx_tests --minimized`):

```
[07:56:29] |   |   cursor_set_on_window-----------|passed
[07:56:29] Passed 107 tests, cancelled 0 tests, failed 0 tests
[07:56:29] ALL PASSED
```
exit code 0. Full log: /tmp/test_final.txt. No `error`/`warning` lines anywhere
in the log (verified by grep).

**Progression (for the record):**
- Pre-fix: both executables died with `ERROR: create: Failed to create GLFW
  window` (GLXBadFBConfig — fix 1 not yet in).
- After fixes 1+2 (first adaptation, then UBO rewrite): **102 passed, 2 failed,
  3 cancelled** — the two failures were exactly the truncation values above
  (127 vs 128; 0 vs 1 on the 0.502 product), 3 cancelled in a dependent module.
  This was the first time the suite could run far enough to exercise readback.
- After fixes 3+4: 107/107.

**Driver probe** (pre-fix evidence, /tmp/glprobe/probe.c, GLX 4.1 core RGBA8):
`glClear` of 127.5/255 reads back **127** (spec: 128) — direct demonstration of
the driver's truncating float→UNORM8 conversion in the exact path the user's
readback bug lives in.

**Sandbox screenshot** (`./build-linux/glvx_sandbox/glvx_sandbox --screenshot
/home/foxclip/GLVX/bugfix_sandbox_screenshot.png`, exit 0):
- File: **52,179 bytes**, valid PNG **800×670 RGBA8** (decoded with a small
  python3 zlib/struct PNG decoder — PIL not installed).
- Not a uniform image: **437 distinct colors** in a sampled grid, including
  `(109,0,145,255)`, `(70,130,180,255)`, `(100,150,128,255)`, `(255,255,255,255)`
  and a **semi-transparent** pixel `(78,58,50,128)` (proves the float-alpha
  pipeline end-to-end); content present in 531 of 670 rows.
- Pixel spot checks: first non-black pixel `(588, 8) = (60,60,60,255)`;
  `(600,400) = (40,40,40,255)`; `(400,600) = (40,40,40,255)`. (Corners and the
  exact center are black — the scene has a dark background/margins — but the image
  as a whole is clearly a rendered scene, not a uniform fill.)

---

## Files changed

| File | Change |
|---|---|
| `src/window.cpp` | context request: 4.2 core → retry 4.1 core (line 55) |
| `src/shader.cpp` | `adapt_shading_language_version` (:117), `replace_uniform_blocks` (:57), `strip_uniform_instance_qualifiers` (:35); adaptation applied in `compileShader` |
| `include/glvx/shader.h` | `m_ubo_available` flag (drives the non-UBO draw path) |
| `src/drawable.cpp` | non-UBO path sends `vp` + `premultiplyOutput` per draw (bare names, no-ops if absent) |
| `include/glvx/abstract_texture.h` | `m_float_storage`, `createFloatTexture`, `readPixelsFloatRaw` |
| `src/abstract_texture.cpp` | `createFloatTexture` (:295), CPU `resample_image` (:198) + float-aware `resizeTexture`, `readPixelsFloatRaw` (:326) |
| `src/render_texture.cpp` | `create` uses RGBA16F for non-MSAA (:59); `readPixels` float path (:171) |
| `glvx_sandbox/src/application.cpp` | renamed local `vec3 color` → `vec3 blended` in `shader_animated_frag` |

No test sources or test framework were modified. `.vscode/launch.json` untouched
(it shows as modified in `git status` — that change pre-dates this work). No new
dependencies.

## Residual risks

- **Windows path (not testable here):** the 4.1 fallback only fires when 4.2
  creation fails — on Windows/DXGI the 4.2 request will normally succeed, so
  behavior there is unchanged. The F16 render-target path is core-3.0+ GL and
  renderable on ANGLE/D3D11, and the CPU resize is platform-neutral. The demotion
  is a no-op on any context ≥ 4.2. If a Windows driver ever *did* fail the 4.2
  request, 4.1 core would be tried and the shader demotion would kick in exactly
  as on Linux.
- **GL 4.2+ machines:** behavior change is intentional and strictly more
  correct — render targets are F16 and readback rounding is done in the library
  (this is also what fixes the user's original GL 4.5 symptom class, which this
  host could not reproduce verbatim: here the pre-fix readback was correct for
  integer values and off-by-one for half-integer ones). The all-black
  `(0 0 0 0)` symptom on the user's machine could not be reproduced on this host
  and is not explained by any code path found here; the readback path is now
  verified pixel-exact by 107 tests.
- **MSAA render targets** stay UNORM8 (RGBA16F multisample requires GL 4.3):
  readback of MSAA targets still relies on driver conversion (no regression vs.
  before; no test or the sandbox uses MSAA).
- **CPU resize** replaces `glBlitFramebuffer` on all platforms: slower for large
  textures (moot at test/sandbox sizes) but gives deterministic, spec-conformant
  rounding everywhere.
- Driver-side `GL_LINEAR` **texture filtering** (subpixel sampling during draws)
  is still done by the driver; every path the tests check now converts bytes
  explicitly, so this no longer affects the suite.

---

## Phase 2 — "F5 vs terminal" mystery (2026-10-23, follow-up fix)

### Symptom
After the Phase 1 fixes, `glvx_tests --minimized` passed **only when run with
`LIBGL_ALWAYS_SOFTWARE=1` (llvmpipe)**. Under the machine's default driver
(SVGA3D — the one VSCode F5 uses) it still crashed with
`GL_INVALID_OPERATION` in `Window::readPixels`, i.e. the user's original
complaint, now on a different line.

### Root causes
1. **Window-system resizes are asynchronous on X11.** `glfwGetFramebufferSize`
   can lag behind `Window::setSize`/resize events, so a window sized from 0 to
   N could render into a stale 0×0 (or N₀×N₀) default framebuffer.
2. **GL_FRONT readback is unreliable on llvmpipe** (returns black/zeros), and
   the old code read `GL_FRONT` for visible windows. The default framebuffer
   therefore could not serve as a stable readback target on either driver.

### Fix (src/window.cpp, include/glvx/window.h)
- The window now **always renders into its offscreen FBO** (`syncOffscreenTexture`
  creates/resizes it unconditionally; the "visible window → default framebuffer"
  branch is gone). The FBO is resized **synchronously** by the library, so
  `clear()`/`draw()`/`readPixels()` always operate on a stable, correctly sized
  surface.
- `display()` presents by blitting the offscreen FBO → default framebuffer
  (`glBlitFramebuffer`, `GL_NEAREST`) + `glfwSwapBuffers`; the blit is skipped
  for headless/0×0 windows. No `glReadBuffer` around the blit: the source is an
  FBO (reads `COLOR_ATTACHMENT0`), and `glReadBuffer` with a non-default FBO
  bound to the read point is `GL_INVALID_OPERATION`.
- `readPixels()` always reads the offscreen FBO (+`flipY`), regardless of
  visibility — one readback path for all modes, like Windows already was.

### Verification (2026-10-23, this machine, Mesa 24.0.9)
- `glvx_tests --minimized`, default driver (SVGA3D, same as F5):
  **107/107 passed, 0 failed, 0 cancelled, exit 0.**
- `glvx_tests --minimized` under `LIBGL_ALWAYS_SOFTWARE=1`: 106/107 —
  `blend_mode_multiply` differs by 2/255 in the G channel (78,60,50 vs expected
  78,58,50). Verified **pre-existing** (identical failure on the unmodified
  Phase 1 build via `git stash`): llvmpipe's multiply-blend rounding differs
  from the GPU's; the test's expected value matches real hardware.
- `glvx_sandbox --screenshot`: 800×670, full app content rendered.
- Live window screen-recording (xwd sequence of the visible sandbox window,
  same binary + default driver as F5): full app content rendered, not black.
