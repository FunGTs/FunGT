#version 440


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 color_position;
layout (location = 2) in vec2 texture_position;
out vec2 pos; 
uniform float frameSizeX;
uniform float frameSizeY;
//Here come the uniform value form the matrices
uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix;


void main(){
    //ProjectionMatrix*ViewMatrix*ModelMatrix*
    gl_Position = vec4(vertex_position,1.f);
    // vec2 ndcPos = gl_Position.xy/gl_Position.w;
    // pos.x = frameSizeX*(ndcPos.x*0.5+0.5); 
    // pos.y = frameSizeY*(ndcPos.y*0.5+0.5);
}