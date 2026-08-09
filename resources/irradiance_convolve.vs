#version 460
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    localPos = aPos;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(aPos, 1.0);
}
