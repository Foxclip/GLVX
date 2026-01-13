#include <iostream>
#include <test_lib/test.h>
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
    void testVector2Constructors(test::Test& test);
    void testVector2Operators(test::Test& test);
    void testColorConstants(test::Test& test);
    void testVertexConstruction(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    addTest("Vector2 Constructors", [&](test::Test& test) { testVector2Constructors(test); });
    addTest("Vector2 Operators", [&](test::Test& test) { testVector2Operators(test); });
    addTest("Color Constants", [&](test::Test& test) { testColorConstants(test); });
    addTest("Vertex Construction", [&](test::Test& test) { testVertexConstruction(test); });
}

void GlvisTestModule::testVector2Constructors(test::Test& test) {
    glvis::Vector2 v1;
    T_COMPARE(v1.x, 0.0f);
    T_COMPARE(v1.y, 0.0f);

    glvis::Vector2 v2(1.5f, -2.3f);
    T_COMPARE(v2.x, 1.5f);
    T_COMPARE(v2.y, -2.3f);
}

void GlvisTestModule::testVector2Operators(test::Test& test) {
    glvis::Vector2 a(1.0f, 2.0f);
    glvis::Vector2 b(3.0f, 4.0f);

    glvis::Vector2 sum = a + b;
    T_COMPARE(sum.x, 4.0f);
    T_COMPARE(sum.y, 6.0f);

    glvis::Vector2 diff = b - a;
    T_COMPARE(diff.x, 2.0f);
    T_COMPARE(diff.y, 2.0f);

    glvis::Vector2 scaled = a * 2.0f;
    T_COMPARE(scaled.x, 2.0f);
    T_COMPARE(scaled.y, 4.0f);

    glvis::Vector2 divided = b / 2.0f;
    T_COMPARE(divided.x, 1.5f);
    T_COMPARE(divided.y, 2.0f);
}

void GlvisTestModule::testColorConstants(test::Test& test) {
    glvis::Color white = glvis::Color::White;
    T_COMPARE(white.r, 255);
    T_COMPARE(white.g, 255);
    T_COMPARE(white.b, 255);
    T_COMPARE(white.a, 255);

    glvis::Color black = glvis::Color::Black;
    T_COMPARE(black.r, 0);
    T_COMPARE(black.g, 0);
    T_COMPARE(black.b, 0);
    T_COMPARE(black.a, 255);
}

void GlvisTestModule::testVertexConstruction(test::Test& test) {
    glvis::Vertex v;
    T_COMPARE(v.position.x, 0.0f);
    T_COMPARE(v.position.y, 0.0f);
    T_COMPARE(v.color.r, 0);
    T_COMPARE(v.color.g, 0);
    T_COMPARE(v.color.b, 0);
    T_COMPARE(v.color.a, 255);
    T_COMPARE(v.texCoords.x, 0.0f);
    T_COMPARE(v.texCoords.y, 0.0f);

    glvis::Vertex v2(glvis::Vector2(1.0f, 2.0f), glvis::Color::Red, glvis::Vector2(0.5f, 0.5f));
    T_COMPARE(v2.position.x, 1.0f);
    T_COMPARE(v2.position.y, 2.0f);
    T_COMPARE(v2.color.r, 255);
    T_COMPARE(v2.color.g, 0);
    T_COMPARE(v2.color.b, 0);
    T_COMPARE(v2.color.a, 255);
    T_COMPARE(v2.texCoords.x, 0.5f);
    T_COMPARE(v2.texCoords.y, 0.5f);
}

int main() {
    test::TestModule root("GLVis Tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Core");
    root.run();

    // TODO: rename tests folder to glvis_tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
