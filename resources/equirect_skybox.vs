#version 460
layout (location = 0) in vec3 aPos;

out vec3 localPos;

layout(std140, binding = 0) uniform Matrices {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

void main()
{
    localPos = aPos;
    mat4 viewNoTranslation = mat4(mat3(ViewMatrix));
    vec4 pos = ProjectionMatrix * viewNoTranslation * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}
