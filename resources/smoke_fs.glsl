#version 440
out vec4 FragColor;

void main() {
    // gl_PointCoord gives texture coordinates (0,0) to (1,1) for the point sprite
    vec2 coord = gl_PointCoord * 2.0 - 1.0; // Map to -1 to 1
    float dist = length(coord);
    
    // Soft circular falloff
    float alpha = 1.0 - smoothstep(0.4, 1.0, dist);
    
    // Smoke color (white/gray with some variation)
    float id = float(gl_PrimitiveID);
    float brightness = 0.7 + 0.3 * fract(sin(id) * 43758.5453);
    vec3 color = vec3(brightness);
    
    // Discard fragments outside the circle
    if (alpha < 0.01) discard;
    
    FragColor = vec4(color, alpha * 0.3); // Low alpha for transparency
}