add_subdirectory(cpp-test-lib)

add_executable(glvis_tests
    main.cpp
)
target_link_libraries(glvis_tests glvis test_lib)
target_include_directories(glvis_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
