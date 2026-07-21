#pragma once

namespace glvis {
namespace shaders {

inline const char* subpixel_vert = R"(
#version 420 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;

layout(binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

void main() {
    gl_Position = object.vp * object.model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
}
)";

inline const char* subpixel_frag = R"(
#version 420 core

in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

layout(binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

uniform sampler2D tex;

void main() {
    vec4 colorNormalized = object.color / 255.0;
    if (object.hasTexture) {
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
