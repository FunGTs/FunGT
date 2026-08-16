#version 440
layout (location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Matrices {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

layout(std430, binding = 2) readonly buffer ModelMatrices {
    mat4 models[];
};

uniform int ModelMatrixIndex;

void main() {
    mat4 ModelMatrix = models[ModelMatrixIndex];
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);
    gl_PointSize = 3.0;
}