#version 410

struct Material{
    //Color data    
    vec3 ambientLight; 
    vec3 diffLigth; 
    vec3 specLight;
    sampler2D diffTexture; 
    sampler2D specTexture;

};


in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal; 

out vec4 fs_color;

uniform Material material; 

/* material.ambientLight;
material.diffLigth;
material.specLight;
material.diffTexture;
material.specTexture;
 */
uniform vec3 lightPos0; 
uniform vec3 cameraPos;
void main(){


    //fs_color = vec4(vs_color,1.f);
    //fs_color = texture(texture0,vs_texcoord);


    //ambient light
    vec3 ambientLight = material.ambientLight;


    //DIFF
    vec3 posToLightDirVec = normalize(vs_position-lightPos0); 
    vec3 diffColor = vec3(1.0f, 1.0f, 1.0f);
    float diffuse = clamp(dot(posToLightDirVec, vs_normal),0,1); 
    vec3 finalDiffuse = material.diffLigth*diffuse;

    //Specualr light:
    

    vec3 lightToPosDirVec = normalize(lightPos0-vs_position);
    vec3 reflecDirVector = normalize(reflect(lightToPosDirVec,normalize(vs_normal))); 
    vec3 PosToViewDirVec = normalize(vs_position-cameraPos);
    float specularConstant = pow(max(dot(PosToViewDirVec,reflecDirVector),0),35);
    vec3 finalSpecLight= material.specLight * specularConstant; 

    //Attenuation

    //Final light

    fs_color = texture(material.diffTexture, vs_texcoord)*vec4(vs_color,1.f)*(vec4(ambientLight,1.f) + vec4(finalDiffuse,1.f) + vec4(finalSpecLight,1.f));

    
}