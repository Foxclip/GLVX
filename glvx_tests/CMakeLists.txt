enable_testing()

add_subdirectory(cpp-test-lib)

add_executable(glvx_tests)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(glvx_tests PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/glvx_tests_common.cpp
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
    ${SRC_DIR}/render_texture_tests.cpp
    ${SRC_DIR}/shader_tests.cpp
    ${SRC_DIR}/antialiasing_tests.cpp
    ${SRC_DIR}/input_tests.cpp
)
target_include_directories(glvx_tests PRIVATE ${INCLUDE_DIR})
target_link_libraries(glvx_tests glvx test_lib)

add_test(NAME glvis_test COMMAND glvx_tests)
