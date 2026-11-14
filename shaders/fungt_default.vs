#version 440

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal_position;
layout(location = 2) in vec2 texture_position;
layout(location = 3) in vec4 color;
layout(location = 4) in vec4 tangent_position;

out vec3 FragPos;
out vec3 Normal;
out vec2 textureCoords;
out vec4 vertexColor;
out mat3 TBN;

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
    mat3 normalMatrix = mat3(transpose(inverse(ModelMatrix)));
    vec3 N = normalize(normalMatrix * normal_position);
    vec3 T = normalize(mat3(ModelMatrix) * tangent_position.xyz);
    T = normalize(T - N * dot(N, T));
    vec3 B = normalize(cross(N, T)) * tangent_position.w;
    Normal = N;
    TBN = mat3(T, B, N);
    textureCoords = texture_position;
    vertexColor = color;
    
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(FragPos, 1.0);
}
