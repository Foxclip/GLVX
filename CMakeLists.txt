cmake_minimum_required(VERSION 3.10)

project(glvis LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)

set(GLM_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(freetype)
add_subdirectory(glvis_tests)

add_library(${PROJECT_NAME})
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Prevent GLFW from auto-including OpenGL headers; we use glad instead
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

target_sources(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/abstract_texture.cpp
    ${SRC_DIR}/angle.cpp
    ${SRC_DIR}/circle.cpp
    ${SRC_DIR}/color.cpp
    ${SRC_DIR}/cursor.cpp
    ${SRC_DIR}/drawable.cpp
    ${SRC_DIR}/glvis_common.cpp
    ${SRC_DIR}/font.cpp
    ${SRC_DIR}/image.cpp
    ${SRC_DIR}/keyboard.cpp
    ${SRC_DIR}/matrix.cpp
    ${SRC_DIR}/float_rect.cpp
    ${SRC_DIR}/mouse.cpp
    ${SRC_DIR}/rectangle.cpp
    ${SRC_DIR}/render_states.cpp
    ${SRC_DIR}/render_target.cpp
    ${SRC_DIR}/render_texture.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/shape.cpp
    ${SRC_DIR}/text.cpp
    ${SRC_DIR}/texture.cpp
    ${SRC_DIR}/transform.cpp
    ${SRC_DIR}/transformable.cpp
    ${SRC_DIR}/utils.cpp
    ${SRC_DIR}/vector.cpp
    ${SRC_DIR}/vertex_array.cpp
    ${SRC_DIR}/vertex_buffer.cpp
    ${SRC_DIR}/uniform_buffer.cpp
    ${SRC_DIR}/view.cpp
    ${SRC_DIR}/window.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${INCLUDE_DIR}
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC freetype)
