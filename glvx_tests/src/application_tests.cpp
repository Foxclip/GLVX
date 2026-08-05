#include "glvx_tests/application_tests.h"

ApplicationTestsModule::ApplicationTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    }
}

void ApplicationTestsModule::clearTest(test::Test& test) {
}
