if(WIN32)
    add_executable(glvx_sandbox WIN32)
    set_target_properties(glvx_sandbox PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
else()
    add_executable(glvx_sandbox)
endif()
target_sources(glvx_sandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
)
target_include_directories(glvx_sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(glvx_sandbox PRIVATE glvx)
