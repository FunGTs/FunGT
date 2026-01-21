#version 440

// ============================================================================
// FunGT Default Shader - Universal Material & Texture Support
// Handles: Materials only, Textures only, or Both
// ============================================================================

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float emission;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 textureCoords;

// Output
out vec4 vs_color;

// Uniforms
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

// Texture support
uniform sampler2D texture_diffuse1;
uniform bool hasTexture;

void main() {
    // ========================================================================
    // 1. GET BASE COLOR (Texture or Material)
    // ========================================================================
    vec3 baseColor;
    if (hasTexture) {
        // Use texture as base color
        baseColor = texture(texture_diffuse1, textureCoords).rgb;
    } else {
        // Use material diffuse as base color
        baseColor = material.diffuse;
    }
    
    // ========================================================================
    // 2. AMBIENT LIGHTING
    // ========================================================================
    vec3 ambient = light.ambient * material.ambient * baseColor;
    
    // ========================================================================
    // 3. DIFFUSE LIGHTING
    // ========================================================================
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * baseColor;
    
    // ========================================================================
    // 4. SPECULAR LIGHTING
    // ========================================================================
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    // ========================================================================
    // 5. COMBINE LIGHTING
    // ========================================================================
    vec3 emissive = baseColor * material.emission;
    vec3 result = ambient + diffuse + specular + emissive;
    vs_color = vec4(result, 1.0);
}