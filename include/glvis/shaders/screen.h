#pragma once

namespace glvis {
namespace shaders {

const char* screen_vert = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 Color;

void main() {
    gl_Position = vec4(aPos.x - 1.0, aPos.y - 1.0, 0.0, 1.0);
    Color = aColor;
    TexCoords = aTexCoords;
}
)";

const char* screen_frag = R"(
#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, TexCoords) * Color;
}
)";

} // namespace shaders
} // namespace glvis
