#version 440 core

in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_color;

void main() {
    // Darken faces based on normal direction to see edges
    vec3 norm = normalize(Normal);
    
    // Top/bottom faces brighter, sides darker
    float shade = 0.6 + 0.4 * abs(norm.y);
    
    // Add slight variation for side faces
    shade += 0.1 * abs(norm.x);
    shade -= 0.1 * abs(norm.z);
    
    vec3 result = u_color * shade;
    FragColor = vec4(result, 1.0);
}
