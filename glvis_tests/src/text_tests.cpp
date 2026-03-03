#include "glvis_tests/text_tests.h"
#include "glvis/font.h"
#include "glvis/text.h"

TextTestsModule::TextTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto font_test = addTest("font", [&](test::Test& test) { fontTest(test); });
        auto text_test = addTest("dimensions", { font_test }, [&](test::Test& test) { dimensionsTest(test); });
    }
}

void TextTestsModule::fontTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("font");

    Font font("fonts/LiberationSans-Regular.ttf");
    int num_glyphs = font.face->num_glyphs;
    T_COMPARE(num_glyphs, 2587);
}

void TextTestsModule::dimensionsTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("rectangle");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf");
    Text text(&font, "A");
    float width = text.getWidth();
    float height = text.getHeight();
    T_COMPARE(width, 20.0f);
    T_COMPARE(height, 21.0f);
}
