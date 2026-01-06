#version 440
layout (location = 0) in vec3 aPos;

uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

out float vHeight; // Pass to fragment shader if needed

void main() {
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);
    
    // Smoke expands as it rises
    float height_normalized = (aPos.z + 2.0) / 5.0; // 0 at bottom, 1 at top
    float size = 5.0 + height_normalized * 15.0; // Grows from 5 to 20 pixels
    
    gl_PointSize = size;
    vHeight = height_normalized;
}