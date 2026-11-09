#version 460
in vec3 localPos;
out vec4 FragColor;
uniform samplerCube environmentMap;
uniform float roughness;

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

void main()
{
    vec3 N = normalize(localPos);
    if (roughness <= 0.0) {
        FragColor = vec4(textureLod(environmentMap, N, 0.0).rgb, 1.0);
        return;
    }

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    float size = float(textureSize(environmentMap, 0).x);
    float texelSolidAngle = 4.0 * PI / (6.0 * size * size);
    float maxMip = float(textureQueryLevels(environmentMap) - 1);
    float a = roughness * roughness;
    vec3 result = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec3 h = sampleGGX(hammersley(i), roughness);
        vec3 H = normalize(tangent * h.x + bitangent * h.y + N * h.z);
        // Split-sum prefilter assumes V = N.
        float nDotH = max(dot(N, H), 0.0);
        vec3 L = normalize(2.0 * nDotH * H - N);
        float nDotL = max(dot(N, L), 0.0);
        if (nDotL > 0.0) {
            float denominator = nDotH * nDotH * (a * a - 1.0) + 1.0;
            float distribution = a * a / max(PI * denominator * denominator, 1e-8);
            float pdf = max(distribution * 0.25, 1e-8);
            float sampleSolidAngle = 1.0 / (float(SAMPLE_COUNT) * pdf);
            float lod = clamp(0.5 * log2(sampleSolidAngle / texelSolidAngle), 0.0, maxMip);
            result += textureLod(environmentMap, L, lod).rgb * nDotL;
            totalWeight += nDotL;
        }
    }
    FragColor = vec4(result / max(totalWeight, 1e-6), 1.0);
}
