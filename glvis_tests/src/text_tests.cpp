#include "glvis_tests/text_tests.h"
#include "glvis/font.h"

TextTestsModule::TextTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto font_test = addTest("font", [&](test::Test& test) { fontTest(test); });
    }
}

void TextTestsModule::fontTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("font");

    Font font("fonts/LiberationSans-Regular.ttf");
    int num_glyphs = font.face->num_glyphs;
    T_COMPARE(num_glyphs, 2587);
}
