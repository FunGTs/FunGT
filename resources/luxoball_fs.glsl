// FRAGMENT SHADER
#version 440
out vec4 vs_color;
in vec2 textureCoords;
in vec3 Normal;
uniform sampler2D texture_diffuse1;

void main() {
    vec3 textureColor = texture(texture_diffuse1, textureCoords).rgb;
    
    // Simple lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 ambient = textureColor * 0.3;
    vec3 diffuse = textureColor * diff * 0.7;
    
    vs_color = vec4(ambient + diffuse, 1.0);
}