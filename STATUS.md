# GLVX GL 4.1 bugfix — STATUS (FINAL)

**Date:** 2026-10-23
**State: COMPLETE — all acceptance criteria met.**

## Done
1. **Window creation on GL 4.1** — src/window.cpp:55: request 4.2 core, retry 4.1 core.
2. **GLSL 420 UBOs on a 4.1 context** — src/shader.cpp compile-time source
   adaptation: demote `#version` to the context's (`adapt_shading_language_version`,
   :117) and rewrite `layout (binding=N) uniform Name {…}` blocks into plain
   uniforms (`replace_uniform_blocks`, :57; `strip_uniform_instance_qualifiers`,
   :35). No-op on 4.2+ machines. `Shader::m_ubo_available` (include/glvx/shader.h)
   drives the non-UBO draw path in src/drawable.cpp (sends bare-name `vp` +
   `premultiplyOutput` per draw).
3. **Driver float→UNORM8 truncation (the readback bug class)** — render targets
   stored as GL_RGBA16F (`AbstractTexture::createFloatTexture` :295;
   `RenderTexture::create` src/render_texture.cpp:59), byte conversion done in
   library with `lroundf` (`readPixelsFloatRaw` :326,
   `RenderTexture::readPixels` :171).
4. **Driver `glBlitFramebuffer` resample truncation** — CPU `resample_image`
   (src/abstract_texture.cpp:198) with explicit rounding, used by
   `AbstractTexture::resizeTexture`; explicit bindings for read (old) / upload
   (new) textures to avoid the read-after-upload-to-texture-0 bug.
5. **Sandbox shader shadowing** — renamed local `vec3 color` → `vec3 blended`
   (glvx_sandbox/src/application.cpp, `shader_animated_frag`); it shadowed the
   demoted `color` uniform (`color.a` → `vec3` has no `.a`).

## Verification evidence
- Tests (from project root, `--minimized`): **`Passed 107 tests, cancelled 0
  tests, failed 0 tests` / `ALL PASSED`**, exit 0, zero error/warning lines in
  log (log: /tmp/test_final.txt).
- Driver truncation proven on this host with a standalone probe
  (/tmp/glprobe/probe.c): `glClearColor(127.5/255,…)` → readback 127, spec says
  128.
- Sandbox: `glvx_sandbox --screenshot bugfix_sandbox_screenshot.png` exit 0;
  PNG 52,179 bytes, 800×670 RGBA8, **437 distinct sampled colors** (incl. a
  semi-transparent pixel (78,58,50,128)); not a uniform image.
- Full writeup: **/home/foxclip/GLVX/BUGFIX_REPORT.md** (root causes with
  file:line, rejected approaches, evidence, residual risks).

## Remaining
- Nothing blocking. Residual risks documented in the report: Windows path not
  testable here (4.1 fallback only fires when 4.2 fails; demotion is no-op on
  4.2+), MSAA render targets stay UNORM8 (need GL 4.3 for F16 multisample; no
  test/sandbox use), CPU resize is slower than blit for very large textures.
- `git status`: modified .vscode/launch.json is pre-existing (left untouched,
  per contract). All library changes are uncommitted working-tree edits.
