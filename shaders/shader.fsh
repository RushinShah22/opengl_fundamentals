#version 330 core

in vec2 texCoords;
uniform sampler2D text;
uniform sampler2D smily_text;
uniform float visibility;
out vec4 FragColor;
void main() {
    FragColor = mix(texture(text, texCoords), texture(smily_text, texCoords), visibility);
}