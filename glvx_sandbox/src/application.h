#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/circle.h"
#include "glvx/convex_shape.h"
#include "glvx/mouse.h"
#include "glvx/view.h"
#include "glvx/color.h"
#include "glvx/text.h"
#include "glvx/texture.h"
#include "glvx/render_texture.h"
#include "glvx/blend_mode.h"

class Application {
public:
    void init(bool minimized = false);
    void run();
    bool captureScreenshot(const std::string& file_path);

private:
    static const int NUM_TRANSPARENT_RECTANGLES = 10;
    static const int NUM_CURSOR_TYPES = 21;
    inline static const float ARROW_PERIOD_SECONDS = 5.0f;
    inline static const float CURSOR_TILE_W = 34.0f;
    inline static const float CURSOR_TILE_H = 62.0f;
    inline static const float CURSOR_TILE_GAP = 2.0f;
    inline static const float CURSOR_ROW_X = 10.0f;
    inline static const float CURSOR_ROW_Y = 346.0f;
    inline static const float CURSOR_ICON_BOX = 24.0f;
    static const int NUM_BLEND_MODES = 5;
    inline static const float BLEND_CELL_W = 150.0f;
    inline static const float BLEND_ROW_X = 10.0f;
    inline static const float BLEND_ROW_Y = 431.0f;
    inline static const float BLEND_BG_W = 110.0f;
    inline static const float BLEND_BG_H = 70.0f;
    inline static const float BLEND_SRC_W = 60.0f;
    inline static const float BLEND_SRC_H = 40.0f;
    static const int NUM_TEXTURE_INTERP = 2;
    static const int NUM_TEXTURE_WRAP = 4;
    inline static const float TEX_ROW_X = 10.0f;
    inline static const float TEX_ROW_Y = 70.0f;
    inline static const float TEX_CELL = 56.0f;
    inline static const float TEX_CELL_GAP = 16.0f;
    inline static const float TEX_LABEL_GAP = 4.0f;
    static const int NUM_AA_CELLS = 2;
    static const int AA_SAMPLES = 4;
    static const int AA_CELL_W = 120;
    static const int AA_CELL_H = 56;
    inline static const float AA_CELL_GAP = 16.0f;
    inline static const float AA_ROW_X = 10.0f;
    inline static const float AA_ROW_Y = 525.0f;
    inline static const float AA_LABEL_GAP = 4.0f;
    static const int NUM_SHADER_CELLS = 2;
    inline static const float SHADER_WAVE_PERIOD_SECONDS = 2.0f;
    static const int MINIMAP_MARGIN = 10;
    static const int MINIMAP_BORDER = 2;
    inline static const float MINIMAP_SCALE = 0.25f;
    // Frames rendered before a screenshot is taken, so the recursive minimap
    // converges: each frame adds one mirror level at MINIMAP_SCALE, and the
    // initial undefined minimap contents shrink away at the deepest level.
    static const int SCREENSHOT_WARMUP_FRAMES = 8;
    inline static const float VIEW_ZOOM_FACTOR = 1.2f;
    inline static const float MIN_ZOOM = 0.1f;
    inline static const float MAX_ZOOM = 20.0f;

    glvx::Window m_window;
    glvx::View m_view;
    glvx::Rectangle m_rectangle_red{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_green{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_blue{20.0f, 20.0f};
    glvx::Circle m_circle{10.0f};
    glvx::ConvexShape m_hexagon{6};
    glvx::Rectangle m_transparent_rectangles[NUM_TRANSPARENT_RECTANGLES];
    glvx::Rectangle m_rgb_group_rectangles[NUM_TRANSPARENT_RECTANGLES][3];
    glvx::BlendMode m_blend_modes[NUM_BLEND_MODES];
    glvx::Rectangle m_blend_backgrounds[NUM_BLEND_MODES];
    glvx::Rectangle m_blend_sources[NUM_BLEND_MODES];
    glvx::Text m_blend_labels[NUM_BLEND_MODES];
    glvx::Texture m_tex_interp[NUM_TEXTURE_INTERP];
    glvx::Rectangle m_tex_interp_rects[NUM_TEXTURE_INTERP];
    glvx::Text m_tex_interp_labels[NUM_TEXTURE_INTERP];
    glvx::Texture m_tex_wrap[NUM_TEXTURE_WRAP];
    glvx::Rectangle m_tex_wrap_rects[NUM_TEXTURE_WRAP];
    glvx::Text m_tex_wrap_labels[NUM_TEXTURE_WRAP];
    glvx::RenderTexture m_aa_render_textures[NUM_AA_CELLS];
    glvx::Rectangle m_aa_cell_rects[NUM_AA_CELLS];
    glvx::Text m_aa_cell_labels[NUM_AA_CELLS];
    std::unique_ptr<glvx::Shader> m_shader_static;
    std::unique_ptr<glvx::Shader> m_shader_animated;
    glvx::Rectangle m_shader_cell_rects[NUM_SHADER_CELLS];
    glvx::Text m_shader_cell_labels[NUM_SHADER_CELLS];
    glvx::RenderTexture m_minimap_texture;
    glvx::Rectangle m_minimap_border;
    glvx::Rectangle m_minimap_rect;
    glvx::Text m_minimap_label;
    glvx::Font m_font_normal;
    glvx::Font m_font_subpixel;
    glvx::Text m_text_normal;
    glvx::Text m_text_subpixel;
    std::chrono::steady_clock::time_point m_start_time;
    glvx::ConvexShape m_arrow{3};
    glvx::ConvexShape m_mouse_arrow{3};
    glvx::Rectangle m_button_background{120.0f, 30.0f};
    glvx::Text m_button_label;
    int m_button_press_count = 0;

    glvx::Cursor m_cursor_types[NUM_CURSOR_TYPES];
    glvx::Cursor m_arrow_cursor;
    glvx::Texture m_cursor_icons[NUM_CURSOR_TYPES];
    glvx::Rectangle m_cursor_tiles[NUM_CURSOR_TYPES];
    glvx::Rectangle m_cursor_icon_rects[NUM_CURSOR_TYPES];
    glvx::Text m_cursor_labels[NUM_CURSOR_TYPES];
    int m_current_cursor_index = -1;
    bool m_panning = false;
    glvx::Vector2i m_last_pan_pos{0, 0};

    void setupShapes();
    void setupBlendShowcase();
    void setupTextureShowcase();
    void setupAntialiasingShowcase();
    void setupShaderShowcase();
    void setupCursorRow();
    void setupMinimap();
    void layoutMinimap(int window_width, int window_height);
    void updateMinimap();
    void handleEvents();
    void handlePanning(const glvx::Event& event);
    void handleZoom(const glvx::Event& event);
    void updateArrow();
    void updateShaderShowcase();
    void updateMouseArrow();
    void updateButton();
    void updateCursorRow();
    bool isMouseOverButton(const glvx::Vector2f& point_world) const;
    void setButtonLabel();
    static bool loadCursorIcon(
        glvx::Cursor::Type type,
        std::vector<unsigned char>& out_pixels,
        int& out_width,
        int& out_height
    );
    static std::string cursorDisplayName(glvx::Cursor::Type type);
    void render();
};
