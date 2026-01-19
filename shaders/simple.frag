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
