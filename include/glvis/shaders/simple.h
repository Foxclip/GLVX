#pragma once

namespace glvis {
namespace shaders {

inline const char* simple_vert = R"(
#version 330 core
#extension GL_ARB_shading_language_420pack : enable
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;
out vec4 UBOColor;
out float UBOHasTexture;

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
};

layout(std140, binding = 1) uniform PerObject {
    mat4 model;
    vec4 color;
    bool hasTexture;
    int _padding;
};

void main() {
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
    UBOColor = color;
    UBOHasTexture = hasTexture ? 1.0 : 0.0;
}
)";

inline const char* simple_frag = R"(
#version 330 core

in vec2 TexCoords;
in vec4 VertexColor;
in vec4 UBOColor;
in float UBOHasTexture;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
    vec4 colorNormalized = UBOColor / 255.0;
    if (UBOHasTexture != 0.0) {
        FragColor = texture(tex, TexCoords) * VertexColor * colorNormalized;
    } else {
        FragColor = VertexColor * colorNormalized;
    }
}
)";

} // namespace shaders
} // namespace glvis
