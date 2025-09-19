#version 440


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal_position;
layout (location = 2) in vec2 texture_position;

//Here come the uniform value form the matrices
uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 


out vec3 vertexColor; //Color output for the fragment shader
out vec2 textureCoords;

void main(){
    //modifies the position using the matrices:
    gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(vertex_position,1.f);
    //vertexColor = color_position;
    textureCoords =  texture_position;

}