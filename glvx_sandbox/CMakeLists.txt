add_executable(glvx_sandbox)
target_sources(glvx_sandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(glvx_sandbox PRIVATE glvx)
