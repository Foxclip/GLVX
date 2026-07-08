#pragma once

#include "test_lib/test.h"

using namespace glvis;

class RenderTextureTestsModule : public test::TestModule {
public:
    RenderTextureTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void clearTest(test::Test& test);

private:

};
