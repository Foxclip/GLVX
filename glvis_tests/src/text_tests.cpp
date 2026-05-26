#include "glvis_tests/text_tests.h"
#include <iostream>
#include "glvis/font.h"
#include "glvis/text.h"
#include "glvis/utils.h"

TextTestsModule::TextTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto font_test = addTest("font", [&](test::Test& test) { fontTest(test); });
        auto dimensions_test = addTest("dimensions", { font_test }, [&](test::Test& test) { dimensionsTest(test); });
        auto render_character_test = addTest("render character", { dimensions_test }, [&](test::Test& test) { renderCharacterTest(test); });
        auto render_string_test = addTest("render string", { render_character_test }, [&](test::Test& test) { renderStringTest(test); });
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
    window.setTitle("dimensions");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf");
    Text text(&font, "A");
    T_COMPARE(text.getWidth(), 20.0f);
    T_COMPARE(text.getHeight(), 21.0f);

    text.setString("AA");
    T_COMPARE(text.getWidth(), 40.0f);
    T_COMPARE(text.getHeight(), 21.0f);

    text.setString(".");
    T_COMPARE(text.getWidth(), 4.0f);
    T_COMPARE(text.getHeight(), 3.0f);

    text.setString(" ");
    T_COMPARE(text.getWidth(), 8.0f);
    T_COMPARE(text.getHeight(), 0.0f);

    text.setString("A ");
    T_COMPARE(text.getWidth(), 28.0f);
    T_COMPARE(text.getHeight(), 21.0f);

    text.setString("A A");
    T_COMPARE(text.getWidth(), 48.0f);
    T_COMPARE(text.getHeight(), 21.0f);

    text.setString("Q");
    T_COMPARE(text.getWidth(), 21.0f);
    T_COMPARE(text.getHeight(), 27.0f);
}

void TextTestsModule::renderCharacterTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render character");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);
    Text text(&font, "A");
    window.draw(text);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 15;
    int max_height = 15;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
    ##         \n\
   .##.        \n\
   ++++        \n\
   #..#        \n\
  +#  #+       \n\
  #+  +#       \n\
 .#.  .#.      \n\
 +######+      \n\
 #+    +#      \n\
+#      #+     \n\
++      ++     \n\
               \n\
               \n\
               \n\
               \n\
";
    T_COMPARE(actual_ascii, expected_ascii);
}

void TextTestsModule::renderStringTest(test::Test& test) {
        window.setSize(WINDOW_SIZE);
    window.setTitle("render character");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);
    Text text(&font, "..");
    window.draw(text);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 15;
    int max_height = 15;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    // str_to_file(actual_ascii, "ascii.txt");
    std::string expected_ascii = "\
 +#  +#        \n\
 +#  +#        \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
               \n\
";
    T_COMPARE(actual_ascii, expected_ascii);
}

std::string TextTestsModule::imageToAscii(const Image& image, int max_width, int max_height) const {
    int width = image.getWidth();
    int height = image.getHeight();
    std::string result;
    for (int y = 0; y < max_height; y++) {
        for (int x = 0; x < max_width; x++) {
            Color color = image.getPixel(x, y);
            float intensity = (color.r + color.g + color.b) / 3.0f;
            if (intensity > 200) {
                result += "#";
            } else if (intensity > 100) {
                result += "+";
            } else if (intensity > 50) {
                result += ".";
            } else {
                result += " ";
            }
        }
        result += "\n";
    }
    return result;
}
