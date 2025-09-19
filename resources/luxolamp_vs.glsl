#version 440


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal_position;
layout (location = 2) in vec2 texture_position;

//Here come the uniform value form the matrices
uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

//Outputs

out vec3 FragPos; 
out vec3 Normal; 
out vec3 vertexColor; //Color output for the fragment shader
out vec2 textureCoords;

void main(){
    //modifies the position using the matrices:
    FragPos = vec3(ModelMatrix*vec4(vertex_position,1.f)); 
    Normal = mat3(transpose(inverse(ModelMatrix)))*normal_position;
    textureCoords =  texture_position;

    gl_Position = ProjectionMatrix*ViewMatrix*vec4(FragPos,1.f); 
    //vertexColor = color_position;
   
}