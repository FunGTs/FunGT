#version 460
in vec2 texCoords;
out vec4 FragColor;

const float PI = 3.14159265359;
const uint SAMPLE_COUNT = 2048u;

vec2 hammersley(uint index)
{
    uint bits = bitfieldReverse(index);
    return vec2(float(index) / float(SAMPLE_COUNT),
                float(bits) * 2.3283064365386963e-10);
}

vec3 sampleGGX(vec2 xi, float roughness)
{
    float a = roughness * roughness;
    float phi = 2.0 * PI * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(max(1.0 - cosTheta * cosTheta, 0.0));
    return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float geometrySchlickIBL(float cosine, float roughness)
{
    float k = roughness * roughness * 0.5;
    return cosine / max(cosine * (1.0 - k) + k, 1e-6);
}

void main()
{
    float nDotV = max(texCoords.x, 1e-4);
    float roughness = max(texCoords.y, 0.001);
    vec3 V = vec3(sqrt(max(1.0 - nDotV * nDotV, 0.0)), 0.0, nDotV);
    vec2 result = vec2(0.0);

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec3 H = sampleGGX(hammersley(i), roughness);
        float vDotH = max(dot(V, H), 0.0);
        vec3 L = normalize(2.0 * vDotH * H - V);
        float nDotL = max(L.z, 0.0);
        float nDotH = max(H.z, 0.0);
        if (nDotL > 0.0) {
            float geometry = geometrySchlickIBL(nDotV, roughness)
                           * geometrySchlickIBL(nDotL, roughness);
            float visibility = geometry * vDotH / max(nDotH * nDotV, 1e-6);
            float fresnel = pow(1.0 - vDotH, 5.0);
            result += vec2(1.0 - fresnel, fresnel) * visibility;
        }
    }
    FragColor = vec4(result / float(SAMPLE_COUNT), 0.0, 1.0);
}
