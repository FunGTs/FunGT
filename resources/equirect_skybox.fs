#version 460
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D equirectMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 dirToEquirectUV(vec3 dir)
{
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec3 dir = normalize(localPos);
    vec2 uv = dirToEquirectUV(dir);
    FragColor = vec4(texture(equirectMap, uv).rgb, 1.0);
}
