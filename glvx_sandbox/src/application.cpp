#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "application.h"
#include <cmath>
#include <iostream>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static int transparentAlpha(int index) {
    int alpha = static_cast<int>(256 / pow(2, index));
    if (index == 0) {
        alpha = 255;
    }
    return alpha;
}

// Horizontal red->blue gradient. Row 0 of the data renders at the top of the
// shape, so the pattern reads in the natural order.
static void generateGradientPixels(std::vector<unsigned char>& pixels, int width, int height) {
    pixels.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4, 0);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float t = static_cast<float>(x) / static_cast<float>(width - 1);
            std::size_t idx = (static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 4;
            pixels[idx + 0] = static_cast<unsigned char>(255.0f * (1.0f - t));
            pixels[idx + 1] = 0;
            pixels[idx + 2] = static_cast<unsigned char>(255.0f * t);
            pixels[idx + 3] = 255;
        }
    }
}

// Right-pointing triangle (apex at the right-center, base on the left edge) on a
// dark background. Vertically symmetric, so the mirror is only visible
// horizontally.
static void generatePatternPixels(std::vector<unsigned char>& pixels, int width, int height) {
    pixels.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4, 0);
    const float center_y = static_cast<float>(height - 1) / 2.0f;
    const unsigned char dark[4] = {48, 48, 48, 255};
    const unsigned char light[4] = {255, 255, 255, 255};
    for (int y = 0; y < height; y++) {
        float d = std::abs(static_cast<float>(y) - center_y) / center_y;
        float x_max = static_cast<float>(width - 1) * (1.0f - d);
        for (int x = 0; x < width; x++) {
            const unsigned char* c = (static_cast<float>(x) <= x_max) ? light : dark;
            std::size_t idx = (static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 4;
            pixels[idx + 0] = c[0];
            pixels[idx + 1] = c[1];
            pixels[idx + 2] = c[2];
            pixels[idx + 3] = c[3];
        }
    }
}

// Writes the image as an RGBA PNG file. readPixels() data is already
// top-down straight-alpha RGBA, which is exactly the layout stb expects.
static bool writePng(const std::string& file_path, const glvx::Image& image) {
    const int stride = image.getWidth() * 4;
    return stbi_write_png(
        file_path.c_str(),
        image.getWidth(),
        image.getHeight(),
        4,
        image.getData().data(),
        stride
    ) != 0;
}

// Vertex shader shared by the custom fragment shaders below. It must declare
// the same object UBO as the built-in shaders so the window can supply the
// view/projection/model matrices, color and flags (see src/drawable.cpp).
static const char* shader_showcase_vert = R"(
#version 420 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;

layout (binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

void main() {
    gl_Position = object.vp * object.model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
}
)";

// Static: a rainbow gradient computed per fragment from the texture
// coordinates, something the built-in shader cannot do.
static const char* shader_static_frag = R"(
#version 420 core

in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

layout (binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

void main() {
    vec3 colorNormalized = object.color.rgb / 255.0;
    vec3 rainbow = 0.5 + 0.5 * cos(6.2831853 * (TexCoords.x + vec3(0.0, 0.33, 0.67)));
    FragColor = vec4(rainbow * colorNormalized, object.color.a / 255.0);
    if (object.premultiplyOutput) {
        FragColor = vec4(FragColor.rgb * FragColor.a, FragColor.a);
    }
}
)";

// Animated: a sine wave scrolling across the cell, driven by the time uniform
// the sandbox updates every frame.
static const char* shader_animated_frag = R"(
#version 420 core

in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

layout (binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

uniform float time;

void main() {
    vec3 colorNormalized = object.color.rgb / 255.0;
    float wave = 0.5 + 0.5 * sin(6.2831853 * (TexCoords.x * 2.0 - time));
    vec3 color = mix(vec3(0.1, 0.2, 0.5), vec3(1.0, 0.4, 0.2), wave) * colorNormalized;
    FragColor = vec4(color, object.color.a / 255.0);
    if (object.premultiplyOutput) {
        FragColor = vec4(FragColor.rgb * FragColor.a, FragColor.a);
    }
}
)";

// Remap a rectangle's fixed [0,1] texture coordinates to [u0..u1] x [v0..v1]
// so that wrapping modes can be exercised with out-of-range UVs.
static void setQuadUv(glvx::Rectangle& rect, float u0, float v0, float u1, float v1) {
    rect.getVertex(0).tex_coords = glvx::Vector2f(u0, v1);
    rect.getVertex(1).tex_coords = glvx::Vector2f(u0, v0);
    rect.getVertex(2).tex_coords = glvx::Vector2f(u1, v1);
    rect.getVertex(3).tex_coords = glvx::Vector2f(u1, v1);
    rect.getVertex(4).tex_coords = glvx::Vector2f(u0, v0);
    rect.getVertex(5).tex_coords = glvx::Vector2f(u1, v0);
}

bool Application::loadCursorIcon(
    glvx::Cursor::Type type,
    std::vector<unsigned char>& out_pixels,
    int& out_width,
    int& out_height
) {
    out_pixels.clear();
    out_width = 0;
    out_height = 0;

#ifdef _WIN32
    // Loads the same Win32 system cursor resource the library uses for that
    // type (see src/cursor.cpp and glfw's win32_window.c), then copies its
    // color bitmap to RGBA, top-down.
    // In the Windows SDK the IDC_* macros expand to MAKEINTRESOURCE (LPSTR),
    // so cast back to the numeric id, as src/cursor.cpp does.
    DWORD resource_id = 0;
    switch (type) {
        case glvx::Cursor::Type::Arrow:                    resource_id = (DWORD)(uintptr_t)IDC_ARROW; break;
        case glvx::Cursor::Type::ArrowWait:                resource_id = (DWORD)(uintptr_t)IDC_APPSTARTING; break;
        case glvx::Cursor::Type::Wait:                     resource_id = (DWORD)(uintptr_t)IDC_WAIT; break;
        case glvx::Cursor::Type::Text:                     resource_id = (DWORD)(uintptr_t)IDC_IBEAM; break;
        case glvx::Cursor::Type::Hand:                     resource_id = (DWORD)(uintptr_t)IDC_HAND; break;
        case glvx::Cursor::Type::SizeHorizontal:           resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeVertical:             resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeTopLeftBottomRight:   resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomLeftTopRight:   resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeLeft:                 resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeRight:                resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeTop:                  resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeBottom:               resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeTopLeft:              resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomRight:          resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomLeft:           resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeTopRight:             resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeAll:                  resource_id = (DWORD)(uintptr_t)IDC_SIZEALL; break;
        case glvx::Cursor::Type::Cross:                    resource_id = (DWORD)(uintptr_t)IDC_CROSS; break;
        case glvx::Cursor::Type::Help:                     resource_id = (DWORD)(uintptr_t)IDC_HELP; break;
        case glvx::Cursor::Type::NotAllowed:               resource_id = (DWORD)(uintptr_t)IDC_NO; break;
    }

    HCURSOR h_cursor = (HCURSOR)LoadImageW(
        NULL, MAKEINTRESOURCEW(resource_id), IMAGE_CURSOR,
        0, 0, LR_DEFAULTSIZE | LR_SHARED
    );
    if (!h_cursor) {
        return false;
    }

    ICONINFO icon_info = {};
    if (!GetIconInfo(h_cursor, &icon_info)) {
        return false;
    }

    if (icon_info.hbmColor == NULL) {
        // monochrome cursors have no color bitmap
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    BITMAP bitmap = {};
    if (!GetObjectW(icon_info.hbmColor, sizeof(BITMAP), &bitmap)) {
        DeleteObject(icon_info.hbmColor);
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    const int width = bitmap.bmWidth;
    const int height = bitmap.bmHeight;
    std::vector<unsigned char> pixels(
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4
    );

    HDC dc = GetDC(NULL);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = -height; // top-down
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    int retrieved = GetDIBits(
        dc, icon_info.hbmColor, 0, height,
        pixels.data(), &bitmap_info, DIB_RGB_COLORS
    );
    ReleaseDC(NULL, dc);

    DeleteObject(icon_info.hbmColor);
    DeleteObject(icon_info.hbmMask);

    if (retrieved <= 0) {
        return false;
    }

    // GetDIBits yields B,G,R,A memory bytes; we want R,G,B,A
    for (int i = 0; i < width * height; i++) {
        std::swap(
            pixels[static_cast<std::size_t>(i) * 4],
            pixels[static_cast<std::size_t>(i) * 4 + 2]
        );
    }

    out_pixels = std::move(pixels);
    out_width = width;
    out_height = height;
    return true;
#else
    (void)type;
    return false;
#endif
}

std::string Application::cursorDisplayName(glvx::Cursor::Type type) {
    const char* raw_name = "Unknown";
    switch (type) {
        case glvx::Cursor::Type::Arrow:                    raw_name = "Arrow"; break;
        case glvx::Cursor::Type::ArrowWait:                raw_name = "ArrowWait"; break;
        case glvx::Cursor::Type::Wait:                     raw_name = "Wait"; break;
        case glvx::Cursor::Type::Text:                     raw_name = "Text"; break;
        case glvx::Cursor::Type::Hand:                     raw_name = "Hand"; break;
        case glvx::Cursor::Type::SizeHorizontal:           raw_name = "SizeHorizontal"; break;
        case glvx::Cursor::Type::SizeVertical:             raw_name = "SizeVertical"; break;
        case glvx::Cursor::Type::SizeTopLeftBottomRight:   raw_name = "SizeTopLeftBottomRight"; break;
        case glvx::Cursor::Type::SizeBottomLeftTopRight:   raw_name = "SizeBottomLeftTopRight"; break;
        case glvx::Cursor::Type::SizeLeft:                 raw_name = "SizeLeft"; break;
        case glvx::Cursor::Type::SizeRight:                raw_name = "SizeRight"; break;
        case glvx::Cursor::Type::SizeTop:                  raw_name = "SizeTop"; break;
        case glvx::Cursor::Type::SizeBottom:               raw_name = "SizeBottom"; break;
        case glvx::Cursor::Type::SizeTopLeft:              raw_name = "SizeTopLeft"; break;
        case glvx::Cursor::Type::SizeBottomRight:          raw_name = "SizeBottomRight"; break;
        case glvx::Cursor::Type::SizeBottomLeft:           raw_name = "SizeBottomLeft"; break;
        case glvx::Cursor::Type::SizeTopRight:             raw_name = "SizeTopRight"; break;
        case glvx::Cursor::Type::SizeAll:                  raw_name = "SizeAll"; break;
        case glvx::Cursor::Type::Cross:                    raw_name = "Cross"; break;
        case glvx::Cursor::Type::Help:                     raw_name = "Help"; break;
        case glvx::Cursor::Type::NotAllowed:               raw_name = "NotAllowed"; break;
    }

    // Insert a space before each capital except the first so the Text
    // line-breaking (which splits on spaces) can wrap the long names.
    std::string name;
    for (std::size_t i = 0; raw_name[i] != '\0'; i++) {
        if (i > 0 && raw_name[i] >= 'A' && raw_name[i] <= 'Z') {
            name += ' ';
        }
        name += raw_name[i];
    }
    return name;
}

void Application::init(bool minimized) {
    m_window.create(800, 600, "GLVX sandbox", 0, minimized);
    m_view.setPosition(m_window.getCenter());
    m_font_normal.openFromFile("fonts/LiberationSans-Regular.ttf");
    m_font_subpixel.openFromFile("fonts/LiberationSans-Regular.ttf", true);
    m_start_time = std::chrono::steady_clock::now();
    setupShapes();
    setupTextureShowcase();
    setupBlendShowcase();
    setupAntialiasingShowcase();
    setupShaderShowcase();
    setupCursorRow();
    setupMinimap();
}

void Application::setupShapes() {
    m_rectangle_red.setColor(glvx::Color::Red);
    m_rectangle_green.setColor(glvx::Color::Green);
    m_rectangle_blue.setColor(glvx::Color::Blue);
    m_rectangle_red.setPosition(10.0f, 10.0f);
    m_rectangle_green.setPosition(40.0f, 10.0f);
    m_rectangle_blue.setPosition(70.0f, 10.0f);

    m_circle.setColor(glvx::Color::Yellow);
    m_circle.setPosition(10.0f, 40.0f);

    const float hexagon_radius = 10.0f;
    float pi_f = static_cast<float>(std::numbers::pi);
    float point_count_f = static_cast<float>(m_hexagon.getPointCount());
    for (std::size_t i = 0; i < m_hexagon.getPointCount(); ++i) {
        float i_f = static_cast<float>(i);
        const float angle = 2.0f * pi_f * i_f / point_count_f;
        m_hexagon.setPoint(i, glvx::Vector2f(
            hexagon_radius * std::cos(angle),
            hexagon_radius * std::sin(angle)
        ));
    }
    m_hexagon.setColor(glvx::Color(0, 255, 255));
    m_hexagon.setPosition(50.0f, 50.0f);

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        int alpha = transparentAlpha(i);
        m_transparent_rectangles[i].setColor(glvx::Color(255, 0, 0, alpha));
        m_transparent_rectangles[i].setSize(glvx::Vector2f(20.0f, 20.0f));
        m_transparent_rectangles[i].setPosition(
            10.0f + i * 30.0f,
            156.0f
        );
    }

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        int alpha = transparentAlpha(i);
        float x = 10.0f + i * 40.0f;
        m_rgb_group_rectangles[i][0].setColor(glvx::Color(255, 0, 0, alpha));
        m_rgb_group_rectangles[i][1].setColor(glvx::Color(0, 255, 0, alpha));
        m_rgb_group_rectangles[i][2].setColor(glvx::Color(0, 0, 255, alpha));
        for (int c = 0; c < 3; c++) {
            m_rgb_group_rectangles[i][c].setSize(glvx::Vector2f(20.0f, 20.0f));
        }
        m_rgb_group_rectangles[i][0].setPosition(x, 186.0f);
        m_rgb_group_rectangles[i][1].setPosition(x + 10.0f, 186.0f);
        m_rgb_group_rectangles[i][2].setPosition(x + 5.0f, 196.0f);
    }

    m_text_normal.setFont(&m_font_normal);
    m_text_normal.setCharacterSize(10);
    m_text_normal.setString("The quick brown fox jumps over the lazy dog.");
    m_text_normal.setPosition(10.0f, 226.0f);
    m_text_subpixel.setFont(&m_font_subpixel);
    m_text_subpixel.setCharacterSize(10);
    m_text_subpixel.setString("The quick brown fox jumps over the lazy dog.");
    m_text_subpixel.setPosition(10.0f, 246.0f);

    m_arrow.setPoint(0, glvx::Vector2f(12.0f, 0.0f));
    m_arrow.setPoint(1, glvx::Vector2f(-8.0f, -7.0f));
    m_arrow.setPoint(2, glvx::Vector2f(-8.0f, 7.0f));
    m_arrow.setColor(glvx::Color::White);
    m_arrow.setOrigin(0.0f, 0.0f);
    m_arrow.setPosition(10.0f, 276.0f);

    m_mouse_arrow.setPoint(0, glvx::Vector2f(12.0f, 0.0f));
    m_mouse_arrow.setPoint(1, glvx::Vector2f(-8.0f, -7.0f));
    m_mouse_arrow.setPoint(2, glvx::Vector2f(-8.0f, 7.0f));
    m_mouse_arrow.setColor(glvx::Color::Yellow);
    m_mouse_arrow.setOrigin(0.0f, 0.0f);
    m_mouse_arrow.setPosition(40.0f, 276.0f);

    m_button_background.setColor(glvx::Color(70, 130, 180));
    m_button_background.setPosition(10.0f, 306.0f);
    m_button_label.setFont(&m_font_normal);
    m_button_label.setCharacterSize(14);
    setButtonLabel();
}

void Application::setupBlendShowcase() {
    const glvx::BlendMode modes[NUM_BLEND_MODES] = {
        glvx::BlendDefault,
        glvx::BlendAlpha,
        glvx::BlendAdd,
        glvx::BlendMultiply,
        glvx::BlendNone
    };
    const char* names[NUM_BLEND_MODES] = {
        "Default",
        "Alpha",
        "Add",
        "Multiply",
        "None"
    };
    for (int i = 0; i < NUM_BLEND_MODES; i++) {
        m_blend_modes[i] = modes[i];
        float cell_x = BLEND_ROW_X + i * BLEND_CELL_W;
        m_blend_backgrounds[i].setColor(glvx::Color(100, 150, 128));
        m_blend_backgrounds[i].setSize(glvx::Vector2f(BLEND_BG_W, BLEND_BG_H));
        m_blend_backgrounds[i].setPosition(
            cell_x + (BLEND_CELL_W - BLEND_BG_W) / 2.0f,
            BLEND_ROW_Y
        );
        m_blend_sources[i].setColor(glvx::Color(200, 100, 100, 128));
        m_blend_sources[i].setSize(glvx::Vector2f(BLEND_SRC_W, BLEND_SRC_H));
        m_blend_sources[i].setPosition(
            cell_x + (BLEND_CELL_W - BLEND_SRC_W) / 2.0f,
            BLEND_ROW_Y + 15.0f
        );
        m_blend_labels[i].setFont(&m_font_normal);
        m_blend_labels[i].setCharacterSize(10);
        m_blend_labels[i].setString(names[i]);
        m_blend_labels[i].setOrigin(m_blend_labels[i].getWidth() / 2.0f, 0.0f);
        m_blend_labels[i].setPosition(
            cell_x + BLEND_CELL_W / 2.0f,
            BLEND_ROW_Y + BLEND_BG_H + 8.0f
        );
    }
}

void Application::setupTextureShowcase() {
    std::vector<unsigned char> gradient_pixels;
    generateGradientPixels(gradient_pixels, 8, 8);
    std::vector<unsigned char> pattern_pixels;
    generatePatternPixels(pattern_pixels, 16, 16);

    // Interpolation cells: the same small gradient drawn with Nearest vs Linear.
    const glvx::InterpolationType interps[NUM_TEXTURE_INTERP] = {
        glvx::InterpolationType::Nearest,
        glvx::InterpolationType::Linear
    };
    const char* interp_names[NUM_TEXTURE_INTERP] = {
        "Nearest",
        "Linear"
    };
    for (int i = 0; i < NUM_TEXTURE_INTERP; i++) {
        m_tex_interp[i].create(8, 8, gradient_pixels.data(), 4);
        m_tex_interp[i].setInterpolation(interps[i]);

        float cell_x = TEX_ROW_X + i * (TEX_CELL + TEX_CELL_GAP);
        m_tex_interp_rects[i].setSize(TEX_CELL, TEX_CELL);
        m_tex_interp_rects[i].setPosition(cell_x, TEX_ROW_Y);
        m_tex_interp_rects[i].setTexture(&m_tex_interp[i]);

        m_tex_interp_labels[i].setFont(&m_font_normal);
        m_tex_interp_labels[i].setCharacterSize(10);
        m_tex_interp_labels[i].setString(interp_names[i]);
        m_tex_interp_labels[i].setOrigin(m_tex_interp_labels[i].getWidth() / 2.0f, 0.0f);
        m_tex_interp_labels[i].setPosition(
            cell_x + TEX_CELL / 2.0f,
            TEX_ROW_Y + TEX_CELL + TEX_LABEL_GAP
        );
    }

    // Wrapping cells: the same small pattern with each wrapping mode, sampled
    // over UV [0,2] x [0,2] so the out-of-range behavior is visible.
    const glvx::WrappingType wraps[NUM_TEXTURE_WRAP] = {
        glvx::WrappingType::ClampToEdge,
        glvx::WrappingType::Repeat,
        glvx::WrappingType::MirroredRepeat,
        glvx::WrappingType::ClampToBorder
    };
    const char* wrap_names[NUM_TEXTURE_WRAP] = {
        "Clamp",
        "Repeat",
        "Mirror",
        "Border"
    };
    for (int i = 0; i < NUM_TEXTURE_WRAP; i++) {
        m_tex_wrap[i].create(16, 16, pattern_pixels.data(), 4);
        m_tex_wrap[i].setInterpolation(glvx::InterpolationType::Nearest);
        m_tex_wrap[i].setWrapping(wraps[i]);

        float cell_x = TEX_ROW_X + (NUM_TEXTURE_INTERP + i) * (TEX_CELL + TEX_CELL_GAP);
        m_tex_wrap_rects[i].setSize(TEX_CELL, TEX_CELL);
        m_tex_wrap_rects[i].setPosition(cell_x, TEX_ROW_Y);
        m_tex_wrap_rects[i].setTexture(&m_tex_wrap[i]);
        setQuadUv(m_tex_wrap_rects[i], 0.0f, 0.0f, 2.0f, 2.0f);

        m_tex_wrap_labels[i].setFont(&m_font_normal);
        m_tex_wrap_labels[i].setCharacterSize(10);
        m_tex_wrap_labels[i].setString(wrap_names[i]);
        m_tex_wrap_labels[i].setOrigin(m_tex_wrap_labels[i].getWidth() / 2.0f, 0.0f);
        m_tex_wrap_labels[i].setPosition(
            cell_x + TEX_CELL / 2.0f,
            TEX_ROW_Y + TEX_CELL + TEX_LABEL_GAP
        );
    }
}

void Application::setupAntialiasingShowcase() {
    const int sample_counts[NUM_AA_CELLS] = {0, AA_SAMPLES};
    const char* names[NUM_AA_CELLS] = {"MSAA off", "MSAA 4x"};

    // A rotated square: its slanted edges show the difference between
    // rendering with and without multisampling.
    glvx::Rectangle diamond(AA_CELL_H - 12.0f, AA_CELL_H - 12.0f);
    const float diamond_half_size = (AA_CELL_H - 12.0f) / 2.0f;
    diamond.setColor(glvx::Color::Red);
    diamond.setOrigin(diamond_half_size, diamond_half_size);
    diamond.setRotation(glvx::Angle::fromDegrees(30.0f));

    for (int i = 0; i < NUM_AA_CELLS; i++) {
        m_aa_render_textures[i].create(AA_CELL_W, AA_CELL_H, sample_counts[i]);

        glvx::View view;
        view.setPosition(static_cast<float>(AA_CELL_W) / 2.0f, static_cast<float>(AA_CELL_H) / 2.0f);
        m_aa_render_textures[i].setView(view);
        m_aa_render_textures[i].clear(glvx::Color::Black);
        m_aa_render_textures[i].draw(diamond);
        // Resolve the multisample buffer into the texture (no-op without MSAA).
        m_aa_render_textures[i].display();

        float cell_x = AA_ROW_X + i * (AA_CELL_W + AA_CELL_GAP);
        m_aa_cell_rects[i].setSize(AA_CELL_W, AA_CELL_H);
        m_aa_cell_rects[i].setPosition(cell_x, AA_ROW_Y);
        m_aa_cell_rects[i].setTexture(&m_aa_render_textures[i]);

        m_aa_cell_labels[i].setFont(&m_font_normal);
        m_aa_cell_labels[i].setCharacterSize(10);
        m_aa_cell_labels[i].setString(names[i]);
        m_aa_cell_labels[i].setOrigin(m_aa_cell_labels[i].getWidth() / 2.0f, 0.0f);
        m_aa_cell_labels[i].setPosition(
            cell_x + AA_CELL_W / 2.0f,
            AA_ROW_Y + AA_CELL_H + AA_LABEL_GAP
        );
    }
}

void Application::setupShaderShowcase() {
    // Constructed here (not as a member initializer) because compiling the
    // shaders requires the GL context created by Window::create.
    m_shader_static = std::make_unique<glvx::Shader>(shader_showcase_vert, shader_static_frag, true);
    m_shader_animated = std::make_unique<glvx::Shader>(shader_showcase_vert, shader_animated_frag, true);

    glvx::Shader* shaders[NUM_SHADER_CELLS] = {
        m_shader_static.get(),
        m_shader_animated.get()
    };
    const char* names[NUM_SHADER_CELLS] = {
        "Static",
        "Animated"
    };
    for (int i = 0; i < NUM_SHADER_CELLS; i++) {
        float cell_x = AA_ROW_X + (NUM_AA_CELLS + i) * (AA_CELL_W + AA_CELL_GAP);
        m_shader_cell_rects[i].setSize(AA_CELL_W, AA_CELL_H);
        m_shader_cell_rects[i].setPosition(cell_x, AA_ROW_Y);
        m_shader_cell_rects[i].setShader(shaders[i]);

        m_shader_cell_labels[i].setFont(&m_font_normal);
        m_shader_cell_labels[i].setCharacterSize(10);
        m_shader_cell_labels[i].setString(names[i]);
        const float label_width = m_shader_cell_labels[i].getWidth();
        // Center the label on the cell, snapped to whole pixels. Centering an
        // odd-width label would put it at a half-pixel x offset, which makes
        // the 1px-wide 'd' stem straddle a pixel boundary and its ascender
        // vanish under the atlas's linear filtering.
        const float label_left_x = std::round(cell_x + AA_CELL_W / 2.0f - label_width / 2.0f);
        m_shader_cell_labels[i].setOrigin(label_width / 2.0f, 0.0f);
        m_shader_cell_labels[i].setPosition(
            label_left_x + label_width / 2.0f,
            AA_ROW_Y + AA_CELL_H + AA_LABEL_GAP
        );
    }
}

void Application::setupCursorRow() {
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        const glvx::Cursor::Type type = static_cast<glvx::Cursor::Type>(i);
        m_cursor_types[i].loadFromSystem(type);

        std::vector<unsigned char> pixels;
        int icon_width = 0;
        int icon_height = 0;
        if (loadCursorIcon(type, pixels, icon_width, icon_height)) {
            m_cursor_icons[i].create(icon_width, icon_height, pixels.data(), 4);
        }

        const float tile_x = CURSOR_ROW_X + i * (CURSOR_TILE_W + CURSOR_TILE_GAP);
        m_cursor_tiles[i].setSize(CURSOR_TILE_W, CURSOR_TILE_H);
        m_cursor_tiles[i].setPosition(tile_x, CURSOR_ROW_Y);
        m_cursor_tiles[i].setColor(glvx::Color(40, 40, 40));

        m_cursor_icon_rects[i].setSize(CURSOR_ICON_BOX, CURSOR_ICON_BOX);
        m_cursor_icon_rects[i].setPosition(
            tile_x + (CURSOR_TILE_W - CURSOR_ICON_BOX) / 2.0f,
            CURSOR_ROW_Y + 4.0f
        );
        if (m_cursor_icons[i].getID() != 0) {
            m_cursor_icon_rects[i].setTexture(&m_cursor_icons[i]);
        }
        m_cursor_icon_rects[i].setColor(glvx::Color::White);

        m_cursor_labels[i].setFont(&m_font_subpixel);
        m_cursor_labels[i].setCharacterSize(7);
        m_cursor_labels[i].setString(cursorDisplayName(type));
        m_cursor_labels[i].setMaxWidth(CURSOR_TILE_W - 2.0f);
        m_cursor_labels[i].setColor(glvx::Color::White);
        m_cursor_labels[i].setOrigin(m_cursor_labels[i].getWidth() / 2.0f, 0.0f);
        m_cursor_labels[i].setPosition(
            tile_x + CURSOR_TILE_W / 2.0f,
            CURSOR_ROW_Y + 30.0f
        );
    }

    m_arrow_cursor.loadFromSystem(glvx::Cursor::Type::Arrow);
    m_window.setMouseCursor(m_arrow_cursor);
}

void Application::setupMinimap() {
    const int window_width = m_window.getWidth();
    const int window_height = m_window.getHeight();

    // Placeholder contents; updateMinimap() replaces them with a fresh
    // readPixels() capture of the window every frame.
    m_minimap_texture.create(window_width, window_height, nullptr, 4);
    m_minimap_texture.setInterpolation(glvx::InterpolationType::Linear);

    const float minimap_width = static_cast<float>(window_width) * MINIMAP_SCALE;
    const float minimap_height = static_cast<float>(window_height) * MINIMAP_SCALE;
    const float minimap_x = static_cast<float>(window_width) - minimap_width - MINIMAP_MARGIN;
    const float minimap_y = static_cast<float>(MINIMAP_MARGIN);

    m_minimap_border.setColor(glvx::Color(60, 60, 60));
    m_minimap_border.setSize(
        minimap_width + MINIMAP_BORDER * 2.0f,
        minimap_height + MINIMAP_BORDER * 2.0f
    );
    m_minimap_border.setPosition(
        minimap_x - MINIMAP_BORDER,
        minimap_y - MINIMAP_BORDER
    );

    m_minimap_rect.setSize(minimap_width, minimap_height);
    m_minimap_rect.setPosition(minimap_x, minimap_y);
    m_minimap_rect.setTexture(&m_minimap_texture);

    m_minimap_label.setFont(&m_font_normal);
    m_minimap_label.setCharacterSize(10);
    m_minimap_label.setString("Minimap");
    m_minimap_label.setPosition(
        minimap_x,
        minimap_y + minimap_height + MINIMAP_BORDER + 4.0f
    );
}

void Application::updateMinimap() {
    // readPixels() returns the last presented frame (or the offscreen texture
    // when minimized), so the minimap trails the scene by one frame and shows
    // itself in the corner, converging to a recursive fixed point.
    const glvx::Image frame = m_window.readPixels();
    // Same size as the existing texture, so update() replaces the texels in
    // place without recreating the GL texture.
    m_minimap_texture.update(
        frame.getData().data(),
        frame.getWidth(),
        frame.getHeight(),
        4
    );
}

void Application::run() {
    while (m_window.isOpen()) {
        handleEvents();
        // handleEvents may close the window (Closed event), which destroys the
        // GL context. Rendering must not happen after that.
        if (!m_window.isOpen()) {
            break;
        }
        render();
    }
}

bool Application::captureScreenshot(const std::string& file_path) {
    handleEvents();
    // The minimap shows the previous frame and is only re-captured every
    // MINIMAP_CAPTURE_INTERVAL frames, so render several frames to let it
    // warm up before the capture is taken.
    for (int i = 0; i < MINIMAP_CAPTURE_INTERVAL * 3; i++) {
        render();
    }
    glvx::Image image = m_window.readPixels();
    if (!writePng(file_path, image)) {
        std::cerr << "Failed to save screenshot to " << file_path << std::endl;
        return false;
    }
    return true;
}

void Application::handleEvents() {
    glvx::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == glvx::EventType::Closed) {
            m_window.close();
        }
        if (event.type == glvx::EventType::MouseButtonPressed) {
            if (event.mouseButton.button == glvx::Mouse::Button::Left) {
                const glvx::Vector2f click_world =
                    m_window.screenToWorld(event.mouseButton.x, event.mouseButton.y);
                if (isMouseOverButton(click_world)) {
                    m_button_press_count++;
                    setButtonLabel();
                }
            }
            if (event.mouseButton.button == glvx::Mouse::Button::Right) {
                m_panning = true;
                m_last_pan_pos = glvx::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
        }
        if (event.type == glvx::EventType::MouseButtonReleased) {
            if (event.mouseButton.button == glvx::Mouse::Button::Right) {
                m_panning = false;
            }
        }
        if (event.type == glvx::EventType::MouseMoved) {
            handlePanning(event);
        }
        if (event.type == glvx::EventType::MouseWheelScrolled) {
            handleZoom(event);
        }
    }
}

void Application::handlePanning(const glvx::Event& event) {
    if (!m_panning) {
        return;
    }
    const float zoom = m_view.getZoom();
    const float dx = static_cast<float>(event.mouseMove.x - m_last_pan_pos.x);
    const float dy = static_cast<float>(event.mouseMove.y - m_last_pan_pos.y);
    // The view position is the world point at screen center, so moving the
    // view opposite to the mouse drag keeps the grabbed world point under
    // the cursor. The view matrix flips y (world +y is drawn downward), so
    // screen and world deltas share the same sign in both axes.
    m_view.move(-dx / zoom, -dy / zoom);
    m_last_pan_pos = glvx::Vector2i(event.mouseMove.x, event.mouseMove.y);
}

void Application::handleZoom(const glvx::Event& event) {
    const float zoom = m_view.getZoom();
    float new_zoom = (event.mouseWheel.delta > 0.0f)
        ? zoom * VIEW_ZOOM_FACTOR
        : zoom / VIEW_ZOOM_FACTOR;
    if (new_zoom < MIN_ZOOM) {
        new_zoom = MIN_ZOOM;
    }
    if (new_zoom > MAX_ZOOM) {
        new_zoom = MAX_ZOOM;
    }

    // World point under the cursor, from the view transform directly so the
    // result stays correct even within the same event batch as a pan.
    const glvx::Vector2f center = m_view.getPosition();
    const float width = static_cast<float>(m_window.getWidth());
    const float height = static_cast<float>(m_window.getHeight());
    const float cursor_vx = static_cast<float>(event.mouseWheel.x) - width / 2.0f;
    const float cursor_vy = static_cast<float>(event.mouseWheel.y) - height / 2.0f;
    const glvx::Vector2f world_at_cursor(
        center.x + cursor_vx / zoom,
        center.y + cursor_vy / zoom
    );
    const float factor = 1.0f - zoom / new_zoom;
    m_view.setPosition(
        center.x + (world_at_cursor.x - center.x) * factor,
        center.y + (world_at_cursor.y - center.y) * factor
    );
    m_view.setZoom(new_zoom);
}

void Application::updateArrow() {
    const auto now = std::chrono::steady_clock::now();
    const float elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<float>>(now - m_start_time).count();
    const float rotation = std::fmod(elapsed_seconds, ARROW_PERIOD_SECONDS) / ARROW_PERIOD_SECONDS;
    m_arrow.setRotation(glvx::Angle::fromRadians(rotation * 2.0f * static_cast<float>(std::numbers::pi)));
}

void Application::updateShaderShowcase() {
    const auto now = std::chrono::steady_clock::now();
    const float elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<float>>(now - m_start_time).count();
    // glUniform* commands operate on the program currently in use, so the
    // animated shader must be bound before its uniform is updated (as
    // Drawable::renderBase does for its own uniforms).
    m_shader_animated->use();
    // One full wave period per SHADER_WAVE_PERIOD_SECONDS.
    m_shader_animated->setFloat("time", std::fmod(elapsed_seconds, SHADER_WAVE_PERIOD_SECONDS) / SHADER_WAVE_PERIOD_SECONDS);
}

void Application::updateMouseArrow() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    const glvx::Vector2f arrow_pos = m_mouse_arrow.getPosition();
    const float dx = mouse_world.x - arrow_pos.x;
    const float dy = mouse_world.y - arrow_pos.y;
    m_mouse_arrow.setRotation(glvx::Angle::fromRadians(-std::atan2(dy, dx)));
}

bool Application::isMouseOverButton(const glvx::Vector2f& point_world) const {
    const glvx::Vector2f position = m_button_background.getPosition();
    const glvx::Vector2f size = m_button_background.getSize();
    return point_world.x >= position.x &&
           point_world.x <= position.x + size.x &&
           point_world.y >= position.y &&
           point_world.y <= position.y + size.y;
}

void Application::setButtonLabel() {
    m_button_label.setString("Clicks: " + std::to_string(m_button_press_count));
    const glvx::Vector2f position = m_button_background.getPosition();
    m_button_label.setOrigin(
        m_button_label.getWidth() / 2.0f,
        m_button_label.getHeight() / 2.0f + 4.0f
    );
    m_button_label.setPosition(
        position.x + m_button_background.getWidth() / 2.0f,
        position.y + m_button_background.getHeight() / 2.0f
    );
}

void Application::updateButton() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    const bool hovered = isMouseOverButton(mouse_world);
    const bool pressed = hovered && glvx::Mouse::isButtonPressed(glvx::Mouse::Button::Left);

    glvx::Color background_color;
    if (pressed) {
        background_color = glvx::Color(30, 60, 110);
    }
    else if (hovered) {
        background_color = glvx::Color(100, 165, 220);
    }
    else {
        background_color = glvx::Color(70, 130, 180);
    }
    m_button_background.setColor(background_color);

    if (pressed) {
        m_button_label.setColor(glvx::Color(220, 220, 220));
    }
    else {
        m_button_label.setColor(glvx::Color::White);
    }
}

void Application::updateCursorRow() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    int hovered = -1;
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        const glvx::Vector2f position = m_cursor_tiles[i].getPosition();
        const glvx::Vector2f size = m_cursor_tiles[i].getSize();
        if (mouse_world.x >= position.x &&
            mouse_world.x <= position.x + size.x &&
            mouse_world.y >= position.y &&
            mouse_world.y <= position.y + size.y) {
            hovered = i;
            break;
        }
    }

    if (hovered != m_current_cursor_index) {
        if (hovered == -1) {
            m_window.setMouseCursor(m_arrow_cursor);
        } else {
            m_window.setMouseCursor(m_cursor_types[hovered]);
        }
        m_current_cursor_index = hovered;
        for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
            if (i == hovered) {
                m_cursor_tiles[i].setColor(glvx::Color(80, 90, 120));
            } else {
                m_cursor_tiles[i].setColor(glvx::Color(40, 40, 40));
            }
        }
    }
}

void Application::render() {
    // Capture the previous frame before clearing, so the minimap texture is
    // ready when the overlay is drawn at the end of this frame. The capture
    // is throttled (see MINIMAP_CAPTURE_INTERVAL) because glReadPixels is
    // expensive; in between, the overlay reuses the cached texture.
    if (m_minimap_frame_counter % MINIMAP_CAPTURE_INTERVAL == 0) {
        updateMinimap();
    }
    ++m_minimap_frame_counter;

    m_window.setView(m_view);
    m_window.clear(glvx::Color::Black);

    m_window.draw(m_rectangle_red);
    m_window.draw(m_rectangle_green);
    m_window.draw(m_rectangle_blue);

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        m_window.draw(m_transparent_rectangles[i]);
    }
    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        m_window.draw(m_rgb_group_rectangles[i][0]);
        m_window.draw(m_rgb_group_rectangles[i][1]);
        m_window.draw(m_rgb_group_rectangles[i][2]);
    }

    m_window.draw(m_circle);
    m_window.draw(m_hexagon);

    m_window.draw(m_text_normal);
    m_window.draw(m_text_subpixel);

    for (int i = 0; i < NUM_TEXTURE_INTERP; i++) {
        m_window.draw(m_tex_interp_rects[i]);
        m_window.draw(m_tex_interp_labels[i]);
    }
    for (int i = 0; i < NUM_TEXTURE_WRAP; i++) {
        m_window.draw(m_tex_wrap_rects[i]);
        m_window.draw(m_tex_wrap_labels[i]);
    }

    updateArrow();
    m_window.draw(m_arrow);

    updateMouseArrow();
    m_window.draw(m_mouse_arrow);

    updateButton();
    m_window.draw(m_button_background);
    m_window.draw(m_button_label);

    updateCursorRow();
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        m_window.draw(m_cursor_tiles[i]);
        if (m_cursor_icons[i].getID() != 0) {
            m_window.draw(m_cursor_icon_rects[i]);
        }
        m_window.draw(m_cursor_labels[i]);
    }

    for (int i = 0; i < NUM_BLEND_MODES; i++) {
        m_window.draw(m_blend_backgrounds[i]);
        glvx::RenderStates states;
        states.blend_mode = m_blend_modes[i];
        m_window.draw(m_blend_sources[i], states);
        m_window.draw(m_blend_labels[i]);
    }

    for (int i = 0; i < NUM_AA_CELLS; i++) {
        m_window.draw(m_aa_cell_rects[i]);
        m_window.draw(m_aa_cell_labels[i]);
    }

    updateShaderShowcase();
    for (int i = 0; i < NUM_SHADER_CELLS; i++) {
        m_window.draw(m_shader_cell_rects[i]);
        m_window.draw(m_shader_cell_labels[i]);
    }

    // Draw the minimap last in screen space (unit scale, view centered on the
    // window center so world coordinates equal screen pixels), so it stays
    // pinned to the window corner no matter how the main view is panned or
    // zoomed.
    glvx::View screen_view;
    screen_view.setPosition(m_window.getCenter());
    m_window.setView(screen_view);
    m_window.draw(m_minimap_border);
    m_window.draw(m_minimap_rect);
    m_window.draw(m_minimap_label);

    m_window.display();
}
