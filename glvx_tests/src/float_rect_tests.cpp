#include "glvx_tests/float_rect_tests.h"

FloatRectTestsModule::FloatRectTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto overlap_test = addTest("intersects_overlap", [&](test::Test& test) { intersectsOverlapTest(test); });
    auto no_overlap_test = addTest("intersects_no_overlap", [&](test::Test& test) { intersectsNoOverlapTest(test); });
    auto touching_test = addTest("intersects_touching_edges", [&](test::Test& test) { intersectsTouchingEdgesTest(test); });
    auto contained_test = addTest("intersects_contained", [&](test::Test& test) { intersectsContainedTest(test); });
    auto result_test = addTest("intersects_result", { overlap_test }, [&](test::Test& test) { intersectsResultTest(test); });
}

void FloatRectTestsModule::intersectsOverlapTest(test::Test& test) {
    FloatRect a(0.0f, 0.0f, 10.0f, 10.0f);
    FloatRect b(5.0f, 5.0f, 10.0f, 10.0f);
    T_CHECK(a.intersects(b));
    T_CHECK(b.intersects(a));
}

void FloatRectTestsModule::intersectsNoOverlapTest(test::Test& test) {
    FloatRect a(0.0f, 0.0f, 10.0f, 10.0f);
    FloatRect b(20.0f, 20.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(b));
    T_CHECK(!b.intersects(a));

    // b directly above a
    FloatRect c(0.0f, 20.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(c));

    // b directly to the right of a
    FloatRect d(20.0f, 0.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(d));
}

void FloatRectTestsModule::intersectsTouchingEdgesTest(test::Test& test) {
    // Touching at an edge or corner is not an intersection (SFML semantics)
    FloatRect a(0.0f, 0.0f, 10.0f, 10.0f);

    FloatRect b(10.0f, 0.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(b));

    FloatRect c(0.0f, 10.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(c));

    FloatRect d(10.0f, 10.0f, 10.0f, 10.0f);
    T_CHECK(!a.intersects(d));
}

void FloatRectTestsModule::intersectsContainedTest(test::Test& test) {
    FloatRect a(0.0f, 0.0f, 20.0f, 20.0f);
    FloatRect b(5.0f, 5.0f, 5.0f, 5.0f);
    T_CHECK(a.intersects(b));
    T_CHECK(b.intersects(a));
}

void FloatRectTestsModule::intersectsResultTest(test::Test& test) {
    FloatRect a(0.0f, 0.0f, 10.0f, 10.0f);
    FloatRect b(5.0f, 5.0f, 10.0f, 10.0f);
    FloatRect intersection;
    a.intersects(b, intersection);
    T_VEC2_COMPARE(intersection.position, Vector2f(5.0f, 5.0f));
    T_VEC2_COMPARE(intersection.size, Vector2f(5.0f, 5.0f));

    // Non-overlapping produces an empty intersection
    FloatRect c(20.0f, 20.0f, 10.0f, 10.0f);
    FloatRect empty;
    a.intersects(c, empty);
    T_VEC2_COMPARE(empty.position, Vector2f(0.0f, 0.0f));
    T_VEC2_COMPARE(empty.size, Vector2f(0.0f, 0.0f));
}
