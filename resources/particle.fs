#version 440
out vec4 FragColor;

// Simple hash function for pseudo-random colors
float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

void main() {
    // Use primitive ID to generate different colors for each particle
    float id = float(gl_PrimitiveID);
    
    vec3 color = vec3(
        hash(id * 1.0),
        hash(id * 2.0), 
        hash(id * 3.0)
    );
    
    FragColor = vec4(color, 1.0);
}
