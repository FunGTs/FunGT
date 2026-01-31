#version 440 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse;

void main() {
    FragColor = texture(texture_diffuse, TexCoord);
}