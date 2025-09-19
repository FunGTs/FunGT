// VERTEX SHADER
#version 440
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal_position;
layout (location = 2) in vec2 texture_position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 textureCoords;
out vec3 Normal;

void main() {
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.0);
    textureCoords = texture_position;
    Normal = mat3(transpose(inverse(ModelMatrix))) * normal_position;
}