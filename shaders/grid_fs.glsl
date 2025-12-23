#version 460 core

in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform float nearPlane;
uniform float farPlane;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale; // XZ plane
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    
    // Z axis (blue)
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color = vec4(0.0, 0.0, 1.0, 1.0);
    
    // X axis (red)
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color = vec4(1.0, 0.0, 0.0, 1.0);
    
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = ProjectionMatrix * ViewMatrix * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = ProjectionMatrix * ViewMatrix * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linearDepth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - clip_space_depth * (farPlane - nearPlane));
    return linearDepth / farPlane;
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    
    gl_FragDepth = computeDepth(fragPos3D);
    
    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = 1.0 - linearDepth;
    
    // Adaptive scaling: use camera distance from target
    vec3 cameraPos = inverse(ViewMatrix)[3].xyz;
    float camHeight = abs(cameraPos.y);  // Height above ground
    
    // Scale based on camera height (simple and stable)
    // Higher camera = bigger grid squares
    float gridScale = max(0.1, camHeight * 0.2);  // Scale factor
    
    // Two grid levels: main and subdivisions
    vec4 color = grid(fragPos3D, 1.0 / gridScale) * 0.6 + 
                 grid(fragPos3D, 10.0 / gridScale) * 0.3;
    
    color.a *= fading;
    
    FragColor = color;
}