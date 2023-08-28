#version 440

out vec4 vs_color;
in vec3 vertexColor; //input variable from the vertex shader
//uniform vec4 change_color; this takes input from the main program
in vec2 textureCoords;
uniform sampler2D ourTexture;
void main(){

    //vs_color = texture(ourTexture,textureCoords);
    //vs_color = vec4(vertexColor,1.0);
    vs_color = vec4(1.f);
}