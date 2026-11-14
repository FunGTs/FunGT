#version 460
out vec2 texCoords;

void main()
{
    vec2 position = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    texCoords = position;
    gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
}
