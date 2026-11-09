#version 440

struct Material {
    vec3  baseColor;
    float metallic;
    float roughness;
    float reflectance;
    float emission;
};

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
in vec2 textureCoords;
in vec4 vertexColor;

out vec4 vs_color;

uniform vec3     viewPos;
uniform Material material;
uniform sampler2D texture_diffuse1;
uniform bool     hasTexture;
uniform sampler2D metallicRoughnessTexture;
uniform bool     hasMetallicRoughnessTexture;
uniform sampler2D occlusionTexture;
uniform bool     hasOcclusionTexture;
uniform bool     hasVertexColor;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform float maxPrefilterMip;
uniform bool     hasIBL;
uniform float    iblIntensity;
uniform vec3     ambientColor;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(N, H), 0.0);
    float d = nDotH * nDotH * (a2 - 1.0) + 1.0;
    return a2 / max(PI * d * d, 1e-6);
}

float geometrySchlickGGX(float nDotX, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return nDotX / max(nDotX * (1.0 - k) + k, 1e-6);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    return geometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
           geometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 baseColor = material.baseColor;

    if (hasTexture)
        baseColor *= texture(texture_diffuse1, textureCoords).rgb;

    if (hasVertexColor)
        baseColor *= vertexColor.rgb;

    float metallic = material.metallic;
    float roughness = material.roughness;

    if (hasMetallicRoughnessTexture) {
        vec4 metallicRoughness =
            texture(metallicRoughnessTexture, textureCoords);

        roughness *= metallicRoughness.g;
        metallic *= metallicRoughness.b;
    }

    roughness = clamp(roughness, 0.05, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    float occlusion = 1.0;

    if (hasOcclusionTexture)
        occlusion = texture(occlusionTexture, textureCoords).r;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 totalDiffuse  = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numLights; i++)
    {
        vec3  lightDir   = normalize(light[i].position - FragPos);
        float dist   = length(light[i].position - FragPos);
        float attenuation = 1.0 / (dist * dist + 1e-6f);
        vec3  radiance   = light[i].color * light[i].power * attenuation;

        vec3 halfway = normalize(viewDir + lightDir);
        float nDotL = max(dot(norm, lightDir), 0.0);
        float nDotV = max(dot(norm, viewDir), 0.0);
        vec3 f0 = mix(vec3(material.reflectance), baseColor, metallic);
        vec3 fresnel = fresnelSchlick(max(dot(halfway, viewDir), 0.0), f0);
        float distribution = distributionGGX(norm, halfway, roughness);
        float geometry = geometrySmith(norm, viewDir, lightDir, roughness);
        vec3 specular = distribution * geometry * fresnel /
            max(4.0 * nDotV * nDotL, 1e-4);
        vec3 diffuseWeight = (vec3(1.0) - fresnel) * (1.0 - metallic);

        totalDiffuse += radiance * diffuseWeight * baseColor / PI * nDotL;
        totalSpecular += radiance * specular * nDotL;
    }

    vec3 ambient;
    if (hasIBL) {
        float nDotV = max(dot(norm, viewDir), 0.0);
        vec3 f0 = mix(vec3(material.reflectance), baseColor, metallic);
        vec3 fresnel = f0 + (max(vec3(1.0 - roughness), f0) - f0)
            * pow(1.0 - nDotV, 5.0);
        vec3 diffuseWeight = (vec3(1.0) - fresnel) * (1.0 - metallic);
        vec3 diffuseIBL = texture(irradianceMap, norm).rgb * baseColor;
        vec3 reflection = reflect(-viewDir, norm);
        vec3 filteredEnvironment = textureLod(prefilterMap, reflection,
                                              roughness * maxPrefilterMip).rgb;
        vec2 brdf = texture(brdfLUT, vec2(nDotV, roughness)).rg;
        vec3 specularIBL = filteredEnvironment * (fresnel * brdf.x + brdf.y);
        ambient = (diffuseWeight * diffuseIBL + specularIBL) * iblIntensity * occlusion;
    }
    else
        ambient = ambientColor * baseColor * (1.0 - metallic) * occlusion;

    vec3 emissive = baseColor * material.emission;
    vec3 result   = ambient + totalDiffuse + totalSpecular + emissive;

    // Reinhard tonemap + gamma correction: HDR irradiance/light values can exceed 1.0
    // and would otherwise hard-clip to white. This rolls off highlights smoothly instead.
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));

    vs_color = vec4(result, 1.0);
}
