#pragma once

namespace glvis {
namespace shaders {

inline const char* subpixel_vert = R"(
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

inline const char* subpixel_frag = R"(
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
        vec4 texColor = texture(tex, TexCoords);
        vec3 subpixel = texColor.rgb;
        vec4 textColor = VertexColor * colorNormalized;
        FragColor = mix(vec4(vec3(0.0), 1.0), textColor, vec4(subpixel, 1.0));
    } else {
        FragColor = VertexColor * colorNormalized;
    }
}
)";

} // namespace shaders
} // namespace glvis
