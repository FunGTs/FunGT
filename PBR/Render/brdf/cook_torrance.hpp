#if !defined(_COOK_TORRANCE_H_)
#define _COOK_TORRANCE_H_

#include "../../../gpu/include/fgt_cpu_device.hpp"
#include "../../../gpu/data/device_pod.hpp"
#include "../../../Vector/vector3.hpp"

fgt_device_forceinline float D_GGX(float NoH, float roughness) {
    // GGX / Trowbridge-Reitz
    float a = roughness * roughness;
    float a2 = a * a;
    float NoH2 = NoH * NoH;
    float denom = NoH2 * (a2 - 1.0f) + 1.0f;
    denom = fmaxf(denom, 1e-6f);
    return a2 / (M_PI * denom * denom + 1e-8f);
}
fgt_device_forceinline float G_SchlickGGX(float NoV, float k) {
    return NoV / (NoV * (1.0f - k) + k + 1e-8f);
}

fgt_device_forceinline float G_Smith(float NoV, float NoL, float roughness) {
    // UE4 style remapping
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    float gV = G_SchlickGGX(NoV, k);
    float gL = G_SchlickGGX(NoL, k);
    return gV * gL;
}

fgt_device_forceinline fungt::Vec3 F_Schlick(const fungt::Vec3& F0, float VoH) {
    // Schlick approximation for vector F0
    float pow5 = powf(1.0f - VoH, 5.0f);
    return F0 + (fungt::Vec3(1.0f, 1.0f, 1.0f) - F0) * pow5;
}

//  small lerp helper 
fgt_device_forceinline fungt::Vec3 lerp(const fungt::Vec3& a, const fungt::Vec3& b, float t) {
    return a * (1.0f - t) + b * t;
}

// --- Convert MaterialData to PBR params ---
fgt_device_forceinline void materialToPBR(const MaterialData& m, fungt::Vec3& baseColor, float& metallic, float& roughness, fungt::Vec3& F0) {
    baseColor = fungt::Vec3(m.baseColor[0], m.baseColor[1], m.baseColor[2]);
    metallic = fmaxf(0.0f, fminf(m.metallic, 1.0f));
    roughness = fmaxf(0.05f, fminf(m.roughness, 1.0f)); // avoid zero roughness
    // Use provided reflectance as dielectric F0; metals use baseColor as F0
    fungt::Vec3 dielectricF0 = fungt::Vec3(m.reflectance, m.reflectance, m.reflectance);
    F0 = lerp(dielectricF0, baseColor, metallic); // F0 = mix(dielectricF0, baseColor, metallic)
}

// --- Evaluate Cook-Torrance for one light sample ---
fgt_device_forceinline fungt::Vec3 evaluateCookTorrance(
    const fungt::Vec3& N,
    const fungt::Vec3& V,
    const fungt::Vec3& L,
    const MaterialData& mat,
    const fungt::Vec3& radiance // light color/intensity
) {
    fungt::Vec3 baseColor; float metallic, roughness; fungt::Vec3 F0;
    materialToPBR(mat, baseColor, metallic, roughness, F0);

    fungt::Vec3 H = (V + L).normalize();
    float NoL = fmaxf(N.dot(L), 0.0f);
    float NoV = fmaxf(N.dot(V), 0.0f);
    float NoH = fmaxf(N.dot(H), 0.0f);
    float VoH = fmaxf(V.dot(H), 0.0f);

    if (NoL <= 0.0f || NoV <= 0.0f) return fungt::Vec3(0.0f);

    float D = D_GGX(NoH, roughness);
    float G = G_Smith(NoV, NoL, roughness);
    fungt::Vec3 F = F_Schlick(F0, VoH);

    // Specular term (vector)
    fungt::Vec3 numerator = F * (D * G);
    float denom = 4.0f * NoV * NoL + 1e-6f;
    fungt::Vec3 specular = numerator / denom;

    // Diffuse (Lambert) scaled by (1 - F) and (1 - metallic)
    fungt::Vec3 kS = F;
    fungt::Vec3 kD = (fungt::Vec3(1.0f, 1.0f, 1.0f) - kS) * (1.0f - metallic);
    fungt::Vec3 diffuse = (baseColor / M_PI);

    fungt::Vec3 Lo = (kD * diffuse + specular) * radiance * NoL;

    // Add emission (if any) from material (emission is scalar intensity)
    if (mat.emission > 0.0f) {
        Lo += baseColor * mat.emission;
    }

    return Lo;
}
#endif // _COOK_TORRANCE_H_
