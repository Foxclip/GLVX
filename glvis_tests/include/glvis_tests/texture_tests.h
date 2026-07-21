#pragma once

#include "test_lib/test.h"
#include "glvis/abstract_texture.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class TextureTestsModule : public test::TestModule {
public:
    TextureTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void textureTest(test::Test& test);
    void textureAlphaTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);
    void textureInterpolationTest(test::Test& test);
    void textureRenderingInterpolationTest(test::Test& test);
    void textureWrappingTest(test::Test& test);

private:
    static std::string interpToString(InterpolationType t);
    static std::string wrapToString(WrappingType t);

};
