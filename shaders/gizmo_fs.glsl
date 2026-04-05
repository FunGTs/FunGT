#version 330 core

uniform vec4 gizmoColor;

out vec4 fragColor;

void main()
{
    fragColor = gizmoColor;
}