#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 vColor;

void main() {
    gl_Position = u_Projection * u_View * vec4(aPos, 1.0);
    vColor = aColor;
}