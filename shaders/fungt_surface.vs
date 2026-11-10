#version 440

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal_position;
layout(location = 2) in vec3 vertex_color;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;

uniform int ModelMatrixIndex;
layout(std140, binding = 0) uniform Matrices {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};
layout(std430, binding = 2) readonly buffer ModelMatrices {
    mat4 models[];
};

void main() {
    mat4 ModelMatrix = models[ModelMatrixIndex];
    FragPos = vec3(ModelMatrix * vec4(vertex_position, 1.0));
    Normal = mat3(transpose(inverse(ModelMatrix))) * normal_position;
    VertexColor = vertex_color;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(FragPos, 1.0);
}
