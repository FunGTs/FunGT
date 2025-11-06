#version 440

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 vs_color;


in vec3 FragPos;
in vec3 Normal;
in vec2 textureCoords;

uniform vec3 viewPos;
uniform Material material;

void main (){


   
    // Ambient lighting
    vec3 ambient = material.ambient * vec3(0.3);

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.0f, 1.0f, 1.0f)); // Example light direction
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec;

    vec3 lighting = ambient + diffuse + specular;
    vs_color = vec4(lighting, 1.0);


}