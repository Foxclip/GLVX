#pragma once

namespace glvis {
namespace shaders {

inline const char* simple_vert = R"(
#version 420 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;

layout(binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout(binding = 1) uniform PerObject {
    mat4 model;
    vec4 color;
    bool hasTexture;
} perObject;

void main() {
    gl_Position = camera.projection * camera.view * perObject.model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
}
)";

inline const char* simple_frag = R"(
#version 420 core

in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

layout(binding = 1) uniform PerObject {
    mat4 model;
    vec4 color;
    bool hasTexture;
} perObject;

uniform sampler2D tex;

void main() {
    vec4 colorNormalized = perObject.color / 255.0;
    if (perObject.hasTexture) {
        FragColor = texture(tex, TexCoords) * VertexColor * colorNormalized;
    } else {
        FragColor = VertexColor * colorNormalized;
    }
}
)";

} // namespace shaders
} // namespace glvis
