enable_testing()

add_subdirectory(cpp-test-lib)

add_executable(glvis_tests)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(glvis_tests PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/glvis_tests_common.cpp
    ${SRC_DIR}/basic_tests.cpp
)
target_include_directories(glvis_tests PRIVATE ${INCLUDE_DIR})
target_link_libraries(glvis_tests glvis test_lib)

add_test(NAME glvis_test COMMAND glvis_tests)
