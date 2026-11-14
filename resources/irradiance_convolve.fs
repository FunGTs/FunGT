#version 460
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

vec3 faceDirection(int face, vec2 uv)
{
    switch (face) {
        case 0: return vec3( 1.0, -uv.y, -uv.x);
        case 1: return vec3(-1.0, -uv.y,  uv.x);
        case 2: return vec3( uv.x,  1.0,  uv.y);
        case 3: return vec3( uv.x, -1.0, -uv.y);
        case 4: return vec3( uv.x, -uv.y,  1.0);
        default: return vec3(-uv.x, -uv.y, -1.0);
    }
}

float solidAngleIntegral(vec2 uv)
{
    return atan(uv.x * uv.y, sqrt(1.0 + dot(uv, uv)));
}

void main()
{
    vec3 N = normalize(localPos);

    // Integrate every texel of a filtered source mip to limit capture cost.
    // These sample directions stay fixed for every output normal.
    int maxMipLevel = textureQueryLevels(environmentMap) - 1;
    float environmentSize = float(textureSize(environmentMap, 0).x);
    int mipLevel = clamp(int(floor(log2(environmentSize / 32.0))), 0, maxMipLevel);
    int faceSize = textureSize(environmentMap, mipLevel).x;
    float texelWidth = 2.0 / float(faceSize);
    vec3 irradiance = vec3(0.0);

    for (int face = 0; face < 6; ++face) {
        for (int y = 0; y < faceSize; ++y) {
            for (int x = 0; x < faceSize; ++x) {
                vec2 lower = vec2(x, y) * texelWidth - 1.0;
                vec2 upper = lower + texelWidth;
                vec2 center = (lower + upper) * 0.5;
                vec3 direction = normalize(faceDirection(face, center));
                float cosine = max(dot(N, direction), 0.0);
                if (cosine <= 0.0) {
                    continue;
                }

                float solidAngle = solidAngleIntegral(upper)
                    - solidAngleIntegral(vec2(lower.x, upper.y))
                    - solidAngleIntegral(vec2(upper.x, lower.y))
                    + solidAngleIntegral(lower);

                vec3 radiance = textureLod(environmentMap, direction, float(mipLevel)).rgb;
                irradiance += radiance * cosine * solidAngle;
            }
        }
    }

    // Store irradiance / PI; the material shader multiplies this by base color.
    FragColor = vec4(irradiance / PI, 1.0);
}
