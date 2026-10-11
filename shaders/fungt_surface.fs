#version 440

struct Light {
    vec3  position;
    vec3  color;
    float power;
};
layout(std140, binding = 1) uniform LightsBlock {
    Light light[32];
    int   numLights;
};
in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

out vec4 vs_color;

uniform vec3     viewPos;
uniform vec3     ambientColor;

const float PI = 3.14159265359;

void main()
{
    vec3 baseColor = VertexColor;
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 totalDiffuse  = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    float roughness = 0.5;
    float metallic = 0.0;

    for (int i = 0; i < numLights; i++)
    {
        vec3  lightDir   = normalize(light[i].position - FragPos);
        float dist   = length(light[i].position - FragPos);
        float attenuation = 1.0 / (dist * dist + 1e-6f);
        vec3  radiance   = light[i].color * light[i].power * attenuation;

        vec3 halfway = normalize(viewDir + lightDir);
        float nDotL = max(dot(norm, lightDir), 0.0);

        totalDiffuse += radiance * baseColor / PI * nDotL;

        float spec = pow(max(dot(norm, halfway), 0.0), 32.0);
        totalSpecular += radiance * spec * 0.3 * nDotL;
    }

    vec3 ambient = ambientColor * baseColor;
    vec3 result = ambient + totalDiffuse + totalSpecular;

    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));

    vs_color = vec4(result, 1.0);
}
