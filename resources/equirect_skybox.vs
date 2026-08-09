#version 460
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    localPos = aPos;
    vec4 pos = ProjectionMatrix * ViewMatrix * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}
