#version 460

out vec4 vs_color;
in vec2 textureCoords;
uniform sampler2D texture_diffuse1;
void main(){

    vs_color = texture(texture_diffuse1,textureCoords);
}