#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 lightWorldPos;
uniform float gizmoScale;

void main()
{
    vec3 right = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    vec3 up    = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    vec3 worldPos = lightWorldPos
                  + right * aPos.x * gizmoScale
                  + up    * aPos.y * gizmoScale;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(worldPos, 1.0);
}