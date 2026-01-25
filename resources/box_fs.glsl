#version 440 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse;

void main() {
    vec3 norm = normalize(Normal);
    
    // Top/bottom faces brighter, sides darker
    float shade = 0.6 + 0.4 * abs(norm.y);
    
    // Add slight variation for side faces
    shade += 0.1 * abs(norm.x);
    shade -= 0.1 * abs(norm.z);
    
    vec3 color = vec3(0.4f, 0.4f, 0.4f);
    vec3 result = color * shade;
    FragColor = vec4(result, 1.0);
}