enable_testing()

add_subdirectory(cpp-test-lib)

add_executable(glvis_tests)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(glvis_tests PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/glvis_tests_common.cpp
    ${SRC_DIR}/basic_tests.cpp
    ${SRC_DIR}/shape_tests.cpp
    ${SRC_DIR}/transform_tests.cpp
    ${SRC_DIR}/texture_tests.cpp
    ${SRC_DIR}/window_tests.cpp
    ${SRC_DIR}/view_tests.cpp
    ${SRC_DIR}/vertex_buffer_tests.cpp
    ${SRC_DIR}/vertex_array_tests.cpp
    ${SRC_DIR}/render_states_tests.cpp
    ${SRC_DIR}/coordinate_transform_tests.cpp
    ${SRC_DIR}/text_tests.cpp
)
target_include_directories(glvis_tests PRIVATE ${INCLUDE_DIR})
target_link_libraries(glvis_tests glvis test_lib)

add_test(NAME glvis_test COMMAND glvis_tests)
