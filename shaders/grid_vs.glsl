#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Fullscreen quad vertices to [-1, 1]
vec3 unprojectPoint(float x, float y, float z, mat4 view, mat4 proj) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(proj);
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    // Generate fullscreen quad
    vec3 positions[6] = vec3[](
        vec3(-1, -1, 0), vec3(1, -1, 0), vec3(1, 1, 0),
        vec3(-1, -1, 0), vec3(1, 1, 0), vec3(-1, 1, 0)
    );
    
    vec3 p = positions[gl_VertexID];
    
    // Unproject to get near and far points
    nearPoint = unprojectPoint(p.x, p.y, 0.0, ViewMatrix, ProjectionMatrix).xyz; // near plane
    farPoint = unprojectPoint(p.x, p.y, 1.0, ViewMatrix, ProjectionMatrix).xyz;  // far plane
    
    gl_Position = vec4(p, 1.0);
}