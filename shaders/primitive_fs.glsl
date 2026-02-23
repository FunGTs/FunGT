#version 440 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float emission;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse;
uniform vec3 lightPos = vec3(5.0, 5.0, 5.0);
uniform vec3 viewPos = vec3(0.0, 2.0, 5.0);
uniform bool hasTexture;
uniform Material material;
void main() {
    vec3 color;
    if(hasTexture){
        color = texture(texture_diffuse, TexCoord).rgb;
    }
    else{
        color = material.diffuse;
    }
   
    
    // Ambient
    vec3 ambient = 0.3 * color;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * vec3(1.0);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}