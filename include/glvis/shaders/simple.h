#pragma once

namespace glvis {
namespace shaders {

const char* simple_vert = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
}
)";

const char* simple_frag = R"(
#version 330 core

in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec4 color;

void main() {
    vec4 colorNormalized = color / 255.0;
    if (hasTexture) {
        FragColor = texture(tex, TexCoords) * VertexColor * colorNormalized;
    } else {
        FragColor = VertexColor * colorNormalized;
    }
}
)";

} // namespace shaders
} // namespace glvis
