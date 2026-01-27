#version 440 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
    FragPos = vec3(ModelMatrix * vec4(vertex_position, 1.0));
    Normal = mat3(transpose(inverse(ModelMatrix))) * vertex_normal;
    TexCoord = vertex_texcoord;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(FragPos, 1.0);
}