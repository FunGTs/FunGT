#version 440

struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
    float emission;
};

struct Light {
    vec3  position;
    vec3  color;
    float power;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 textureCoords;

out vec4 vs_color;

uniform vec3     viewPos;
uniform Material material;
uniform Light    light[8];
uniform int      numLights;
uniform sampler2D texture_diffuse1;
uniform bool     hasTexture;

void main()
{
    vec3 baseColor;
    if (hasTexture)
        baseColor = texture(texture_diffuse1, textureCoords).rgb;
    else
        baseColor = material.diffuse;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 totalAmbient  = vec3(0.0);
    vec3 totalDiffuse  = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numLights; i++)
    {
        vec3  lightDir   = normalize(light[i].position - FragPos);
        float dist   = length(light[i].position - FragPos);
        float attenuation = 1.0 / (dist * dist + 1e-6f);
        vec3  radiance   = light[i].color * light[i].power * attenuation;

        // Ambient
        totalAmbient += radiance * 0.05 * material.ambient;

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        totalDiffuse += radiance * diff * baseColor;

        // Specular
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        totalSpecular += radiance * spec * material.specular;
    }

    vec3 emissive = baseColor * material.emission;
    vec3 result   = totalAmbient + totalDiffuse + totalSpecular + emissive;
    vs_color = vec4(result, 1.0);
}