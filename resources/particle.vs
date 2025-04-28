#version 440
layout (location = 0) in vec3 aPos;
uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 
void main() {
    gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(aPos, 1.0);
    gl_PointSize = 3.0;// Size of the particle
}