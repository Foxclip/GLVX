#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class TextTestsModule : public test::TestModule {
public:
    TextTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void fontTest(test::Test& test);
    void dimensionsTest(test::Test& test);
    void renderCharacterATest(test::Test& test);
    void renderCharacterDotTest(test::Test& test);
    void renderStringAATest(test::Test& test);
    void transparencyTest(test::Test& test);
    void kerningTest(test::Test& test);
    void descenderTest(test::Test& test);
    void subpixelTest(test::Test& test);

private:

    std::string imageToAscii(const Image& image, int max_width, int max_height) const;
    std::string imageToNumbers(const Image& image, int max_width, int max_height) const;
    std::string imageToNumbersRGB(const Image& image, int max_width, int max_height) const;

};
