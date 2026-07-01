#include "glvis_tests/text_tests.h"
#include <iostream>
#include "glvis/font.h"
#include "glvis/text.h"
#include "glvis/utils.h"

TextTestsModule::TextTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto font_test = addTest("font", [&](test::Test& test) { fontTest(test); });
    auto dimensions_test = addTest("dimensions", { font_test }, [&](test::Test& test) { dimensionsTest(test); });
    auto render_character_A_test = addTest("render character A", { dimensions_test }, [&](test::Test& test) { renderCharacterATest(test); });
    auto render_character_dot_test = addTest("render character dot", { dimensions_test }, [&](test::Test& test) { renderCharacterDotTest(test); });
    auto render_string_AA_test = addTest("render string AA", { render_character_A_test }, [&](test::Test& test) { renderStringAATest(test); });
    auto transparency_test = addTest("transparency", { render_character_A_test }, [&](test::Test& test) { transparencyTest(test); });
    auto kerning_test = addTest("kerning", { dimensions_test }, [&](test::Test& test) { kerningTest(test); });
    auto descender_test = addTest("descender", { dimensions_test }, [&](test::Test& test) { descenderTest(test); });
    auto subpixel_test = addTest("subpixel", { font_test }, [&](test::Test& test) { subpixelTest(test); });
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
    T_COMPARE(text.getWidth(), 46.0f);
    T_COMPARE(text.getHeight(), 21.0f);

    text.setString("Q");
    T_COMPARE(text.getWidth(), 21.0f);
    T_COMPARE(text.getHeight(), 27.0f);
}

void TextTestsModule::renderCharacterATest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render character A");
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
    int max_width = 16;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                \n\
                \n\
                \n\
                \n\
    ##          \n\
   .##.         \n\
   ++++         \n\
   #..#         \n\
  +#  #+        \n\
  #+  +#        \n\
 .#.  .#.       \n\
 +######+       \n\
 #+    +#       \n\
+#      #+      \n\
++      ++      \n\
                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::renderCharacterDotTest(test::Test& test) {
        window.setSize(WINDOW_SIZE);
    window.setTitle("render character dot");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);
    Text text(&font, ".");
    window.draw(text);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 16;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
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
 +#             \n\
 +#             \n\
                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::renderStringAATest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render character AA");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);
    Text text(&font, "AA");
    window.draw(text);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 32;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                                \n\
                                \n\
                                \n\
                                \n\
    ##        ##                \n\
   .##.      .##.               \n\
   ++++      ++++               \n\
   #..#      #..#               \n\
  +#  #+    +#  #+              \n\
  #+  +#    #+  +#              \n\
 .#.  .#.  .#.  .#.             \n\
 +######+  +######+             \n\
 #+    +#  #+    +#             \n\
+#      #++#      #+            \n\
++      ++++      ++            \n\
                                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::transparencyTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("transparency");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);
    Text textA(&font, "A");
    Text textV(&font, "V");
    window.draw(textA);
    window.draw(textV);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 16;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                \n\
                \n\
                \n\
                \n\
++  ##  ++      \n\
.# .##. #.      \n\
 #++++++#       \n\
 +##..##+       \n\
 .##  ##.       \n\
  ##  ##        \n\
 .##  ##.       \n\
 +######+       \n\
 #++++++#       \n\
+# .##. #+      \n\
++  ##  ++      \n\
                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::kerningTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("kerning");

    Font font("fonts/LiberationSans-Regular.ttf", 15);

    Text textA(&font, "A");
    T_COMPARE(textA.getWidth(), 10.0f);
    Text textV(&font, "V");
    T_COMPARE(textV.getWidth(), 10.0f);
    Text text(&font, "AV");
    T_COMPARE(text.getWidth(), 19.0f);

    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Text textAV(&font, "AV");
    window.draw(textAV);
    window.display();

    Image image = window.readPixels();
    int width = image.getWidth();
    int height = image.getHeight();
    int max_width = 32;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                                \n\
                                \n\
                                \n\
                                \n\
    ##   ++      ++             \n\
   .##.  .#      #.             \n\
   ++++   #+    +#              \n\
   #..#   +#    #+              \n\
  +#  #+  .#    #.              \n\
  #+  +#   #+  +#               \n\
 .#.  .#.  +#  #+               \n\
 +######+   #..#                \n\
 #+    +#   ++++                \n\
+#      #+  .##.                \n\
++      ++   ##                 \n\
                                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::descenderTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("descender");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15);

    Text textG(&font, "g");
    Text textY(&font, "y");
    Text textP(&font, "p");
    Text textQ(&font, "q");
    Text textA(&font, "A");

    T_COMPARE(textA.getHeight(), 11.0f);
    T_COMPARE(textG.getHeight(), 11.0f);
    T_COMPARE(textY.getHeight(), 11.0f);
    T_COMPARE(textP.getHeight(), 11.0f);
    T_COMPARE(textQ.getHeight(), 11.0f);

    Text text(&font, "Aq");
    window.draw(text);
    window.display();

    Image image = window.readPixels();
    int max_width = 32;
    int max_height = 19;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                                \n\
                                \n\
                                \n\
                                \n\
    ##                          \n\
   .##.                         \n\
   ++++                         \n\
   #..#     +##.#.              \n\
  +#  #+   #+  +#.              \n\
  #+  +#  .#    #.              \n\
 .#.  .#. .#    #.              \n\
 +######+ .#    #.              \n\
 #+    +# .#   .#.              \n\
+#      #+ #+  +#.              \n\
++      ++  ###.#.              \n\
                #.              \n\
                #.              \n\
                #.              \n\
                                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
}

void TextTestsModule::subpixelTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("subpixel");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Font font("fonts/LiberationSans-Regular.ttf", 15, true);
    T_COMPARE(font.isSubpixel(), true);
    Text text(&font, "A");
    RenderStates states;
    states.shader = glvis::common::defaultShader;
    window.draw(text, states);
    window.display();

    Image image = window.readPixels();
    T_COMPARE(image.getWidth() > 0, true);
    T_COMPARE(image.getHeight() > 0, true);

    int max_width = 16;
    int max_height = 16;
    std::string actual_ascii = imageToAscii(image, max_width, max_height);
    std::string expected_ascii = "\
                \n\
                \n\
                \n\
                \n\
    ##          \n\
   .##+         \n\
   ++++         \n\
   #..#.        \n\
  +#  #+        \n\
  #+  +#        \n\
 .#.  .#.       \n\
 +######+       \n\
 #+    +#       \n\
+#      #+      \n\
++      ++      \n\
                \n\
";
    T_COMPARE_RAW(actual_ascii, expected_ascii);
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

std::string TextTestsModule::imageToNumbers(const Image& image, int max_width, int max_height) const {
    std::string result;
    char buffer[16];
    for (int y = 0; y < max_height; y++) {
        for (int x = 0; x < max_width; x++) {
            Color color = image.getPixel(x, y);
            int intensity = (int)((color.r + color.g + color.b) / 3.0f);
            std::snprintf(buffer, sizeof(buffer), "%4d", intensity);
            result += buffer;
            if (x < max_width - 1) {
                result += " ";
            }
        }
        result += "\n";
    }
    return result;
}
