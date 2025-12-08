#include "model.hpp"
#include "../vendor/stb_image/stb_image.h"
Model::Model()
{
     std::cout<<"Model Default Constructor"<<std::endl;
}

Model::Model(const std::string &path)
{

    std::cout<<"Model Constructor"<<std::endl;
   
}
Model::~Model(){

    std::cout<<"Model Destructor"<<std::endl; 

}

//Methods:

void Model::draw(Shader &shader)
{
    //std::cout<<"Drawing a Model "<<std::endl; 
    for(unsigned int i=0; i<m_vMesh.size(); i++){
        m_vMesh[i]->draw(shader);
    }   
}

void Model::loadModel(const std::string &path)
{
    std::cout << "Assimp Version: "
              << aiGetVersionMajor() << "."
              << aiGetVersionMinor() << "."
              << aiGetVersionRevision() << std::endl;
    std::cout<<"Model Loading"<<std::endl;
    Assimp::Importer import; 
    const aiScene *pScene = import.ReadFile(path, ASSIMP_LOAD_FLAGS); 
    if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode){
        
        std::cout<<"ERROR in ASSIMP :"<<import.GetErrorString()<<std::endl;
        exit(0);  
    }
    m_dirPath = path.substr(0, path.find_last_of('/'));
    
    //Process the Nodes: 
     
    //std::cout<< " Dir : "<< m_dirPath << std::endl; 
    //processNodes(pScene->mRootNode, pScene);

    processAssimpScene(pScene->mRootNode, pScene);
    
    
}

void Model::loadModelData(const std::string& path)
{
    std::cout<<"Loading Model Data "<<std::endl;
    std::cout << "Assimp Version: "
        << aiGetVersionMajor() << "."
        << aiGetVersionMinor() << "."
        << aiGetVersionRevision() << std::endl;
    std::cout << "Model Loading" << std::endl;
    Assimp::Importer import;
    const aiScene* pScene = import.ReadFile(path, ASSIMP_LOAD_FLAGS);
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {

        std::cout << "ERROR in ASSIMP :" << import.GetErrorString() << std::endl;
        exit(0);
    }
    m_dirPath = path.substr(0, path.find_last_of('/'));

    //Process the Nodes: 

    //std::cout<< " Dir : "<< m_dirPath << std::endl; 
    //processNodes(pScene->mRootNode, pScene);

    processAssimpScene(pScene->mRootNode, pScene);
}

void Model::InitGraphics()
{
    switch (DisplayGraphics::GetBackend()) {
    case Backend::OpenGL:
        {
            std::cout<<"Number of meshes: "<<m_vMesh.size()<<std::endl;
            for (auto& mesh : m_vMesh) {
                mesh->InitOGLBuffers();
            }
            break;
        }
        
    case Backend::Vulkan:
        {
            break;
        }
    default:
        throw std::runtime_error("Unknown Graphics API!");
    }
}

std::vector<funGTVERTEX> Model::getVertices(aiMesh *mesh, const aiScene *scene)
{
    std::vector<funGTVERTEX> vertices; 
    //std::cout<<"  Mesh contains:  " <<mesh->mNumVertices << " vertices"<<std::endl; 
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
       
       glm::vec3 auxVec; //to pass the assimp vertex to glm::vec3 
       auxVec.x = mesh->mVertices[i].x; 
       auxVec.y = mesh->mVertices[i].y; 
       auxVec.z = mesh->mVertices[i].z; 
       // puts the glm::vec3 inside our defined Vertex struct
       funGTVERTEX internalVertex; 
       internalVertex.position = auxVec; 
       //overrides the auxVec with the assimp normals
        if(mesh->HasNormals()){
            auxVec.x = mesh->mNormals[i].x;
            auxVec.y = mesh->mNormals[i].y; 
            auxVec.z = mesh->mNormals[i].z; 
            //stores the normals in our defined vertex struct
            internalVertex.normal = auxVec;
        }

       //checks if the mesh has any textures just in the 0 position 
       if(mesh->mTextureCoords[0]){
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            internalVertex.texcoord = vec;

       }
       else{
            internalVertex.texcoord = glm::vec2(0.0f, 0.0f);  
       }

       vertices.push_back(internalVertex); 

    }

    return vertices;
}

std::vector<GLuint> Model::getIndices(aiMesh *mesh, const aiScene *scene)
{   
    std::vector<GLuint> indices; 
    //lets iterate using the faces
    for(int i=0; i<mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i]; 

        //lets extract each index of the current face

        for(int j=0; j<face.mNumIndices; j++){

            indices.push_back(static_cast<GLint>(face.mIndices[j]));

        }
    } 

    return indices;
}

std::vector<Texture > Model::getTextures(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Texture > textures; 
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture > diffuseM  = loadTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");
    textures.insert(textures.end(),diffuseM.begin(),diffuseM.end());
    //std::vector<Texture > specularM  = loadTextures(material,aiTextureType_SPECULAR,"texture_specular");
    //textures.insert(textures.end(),specularM.begin(),specularM.end());



    return textures;
}

std::vector<Material> Model::getMaterials(aiMesh *mesh, const aiScene *scene)
{    
    std::vector<Material> vMaterial;
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    vMaterial = loadMaterials(material);

    return vMaterial;
}

void Model::processNodes(aiNode * node, const aiScene *scene){
    //Recursive function 

    std::cout<<"Processing Nodes and creating the mesh recursively "<<std::endl; 
    std::cout<<"Children Nodes : " <<node->mNumChildren<<std::endl; 
       //std::cout<<" This mRootNode contains " <<node->mNumMeshes << " Meshes"<<std::endl; 
    //process all the node's meshes
    for(unsigned int i=0; i<node->mNumMeshes; i++){
        //Create a temp mesh variable to hold the meshes of the scene 
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        //processMesh(mesh,scene);
        m_vMesh.push_back(processMesh(mesh, scene));
    }
    //our node has children nodes? 
    for(unsigned int i=0; i<node->mNumChildren; i++){
        //std::cout<<"  mRootNode children node :  " <<node->mNumMeshes << " **"<<std::endl; 
        processNodes(node->mChildren[i],scene);
    }


}

void Model::processAssimpScene(aiNode *node, const aiScene *scene)
{
    std::cout<<"Processing Nodes and creating the mesh iteratively "<<std::endl; 
    std::cout<<"Children Nodes : " <<node->mNumChildren<<std::endl; 
    std::stack<aiNode *> aiNodeStackTrack; 
    aiNodeStackTrack.push(node);

    while(!aiNodeStackTrack.empty())/*Do it if is not empty*/{
        aiNode* currentNode; 
        currentNode = aiNodeStackTrack.top();
        aiNodeStackTrack.pop();

        for(unsigned int i=0; i<currentNode->mNumMeshes; i++){
            aiMesh *mesh = scene->mMeshes[currentNode->mMeshes[i]];
            m_vMesh.push_back(processMesh(mesh, scene));

        }
        // Push children nodes onto the stack
        for (unsigned int i = 0; i < currentNode->mNumChildren; i++) {
            aiNodeStackTrack.push(currentNode->mChildren[i]);
        }
    }
    
}

void Model::createShader(std::string vertex_shader, std::string fragment_shader)
{
    m_shader.create(vertex_shader,fragment_shader);
}

const std::vector<std::unique_ptr<Mesh>>& Model::getMeshes()
{
    return m_vMesh;
}

void Model::draw()
{
      //std::cout<<"Drawing a Model "<<std::endl; 
    for(unsigned int i=0; i<m_vMesh.size(); i++){
        m_vMesh[i]->draw(m_shader);
    }   
}

std::unique_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<funGTVERTEX> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> texture;
    std::vector<Material> materials;

    vertices = getVertices(mesh, scene);
    indices = getIndices(mesh, scene);
    texture = getTextures(mesh, scene);
    materials = getMaterials(mesh, scene);
    std::cout << "========== BEFORE FIX ==========" << std::endl;
    std::cout << "Materials loaded : " << materials.size() << std::endl;
    if (!materials.empty()) {
        std::cout << "Material[0] name: " << materials[0].m_name << std::endl;
        std::cout << "Material[0] ambient: " << materials[0].m_ambientLight.x << ", "
            << materials[0].m_ambientLight.y << ", "
            << materials[0].m_ambientLight.z << std::endl;
        std::cout << "Material[0] diffuse: " << materials[0].m_diffLigth.x << ", "
            << materials[0].m_diffLigth.y << ", "
            << materials[0].m_diffLigth.z << std::endl;
        std::cout << "Material[0] specular: " << materials[0].m_specLight.x << ", "
            << materials[0].m_specLight.y << ", "
            << materials[0].m_specLight.z << std::endl;
        std::cout << "isBlackMaterial? " << (materials[0].isInvalidMaterial() ? "YES" : "NO") << std::endl;
    }
    // Fix black materials
    if (!materials.empty() && materials[0].isInvalidMaterial()) {
        std::cout << "Invakid material detected, replacing with FunGT default" << std::endl;
        materials[0] = Material::createDefaultMaterial();
    }

    // Always ensure we have at least one material
    if (materials.empty()) {
        std::cout << "No materials found, adding FunGT default" << std::endl;
        materials.push_back(Material::createDefaultMaterial());
    }
    std::cout << "========== AFTER FIX ==========" << std::endl;
    if (!materials.empty()) {
        std::cout << "Material[0] name: " << materials[0].m_name << std::endl;
        std::cout << "Material[0] ambient: " << materials[0].m_ambientLight.x << ", "
            << materials[0].m_ambientLight.y << ", "
            << materials[0].m_ambientLight.z << std::endl;
        std::cout << "Material[0] diffuse: " << materials[0].m_diffLigth.x << ", "
            << materials[0].m_diffLigth.y << ", "
            << materials[0].m_diffLigth.z << std::endl;
        std::cout << "Material[0] specular: " << materials[0].m_specLight.x << ", "
            << materials[0].m_specLight.y << ", "
            << materials[0].m_specLight.z << std::endl;
    }
    std::cout << "================================" << std::endl;
    std::cout << "Indices loaded   : " << indices.size() << std::endl;
    std::cout << "Vertices loaded  : " << vertices.size() << std::endl;
    std::cout << "Textures loaded  : " << texture.size() << std::endl;
    std::cout << "Materials loaded : " << materials.size() << std::endl;

    // ALWAYS use the combined constructor - handles all cases!
    return std::make_unique<Mesh>(vertices, indices, texture, materials);
}

std::vector<Texture > Model::loadTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    //std::cout<<"Loading : "<<typeName<<std::endl; 
    std::vector<Texture > textures;
    for(unsigned int i = 0; i<mat->GetTextureCount(type); i++){
        aiString str; 
        mat->GetTexture(type,i,&str);
        std::string txt_path = m_dirPath + "/" +  str.C_Str();
        bool wasLoaded = false;
        for(unsigned int j=0; j<m_loadedTextures.size(); j++){
             if(m_loadedTextures[j].getPath().compare(txt_path)==0){
                std::cout<<"Texture already loaded..."<<std::endl; 
                textures.push_back(m_loadedTextures[j]);
                wasLoaded = true;
                break; 
            }
        }
        if(!wasLoaded){
            Texture  internalTexture;
            std::cout<<"Texture not loaded, loading..."<<std::endl; 
            std::cout<<" Texture path : " << txt_path <<std::endl;
             
            //Here  we populate or texture
            internalTexture.genTexture(txt_path);
            std::cout<<"Texture ID : "<<internalTexture.getID()<<std::endl;
            internalTexture.setTypeName(typeName);
            internalTexture.setPath(txt_path);
            textures.push_back(internalTexture);
            m_loadedTextures.push_back(internalTexture);  

        
        } 

        
    } 
    return textures;
}
std::vector<Material> Model::loadMaterials(aiMaterial *mat)
{   std::vector<Material> internalMat; 
    glm::vec3 ka;
    glm::vec3 kd; 
    glm::vec3 ks;
    float shininess;
    

    aiString aiName; 

    //Gets the name of the material: 
    mat->Get(AI_MATKEY_NAME,aiName);


    const char * name = aiName.C_Str();

    std::string nameMaterial(name);

    std::cout<<"Loading : " << nameMaterial <<std::endl;
    
    aiColor3D aiAmbientColor, aiDiffuseColor, aiSpecularColor;
    float aiShininess;
    //Gets the values of the properties
    mat->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbientColor);
    ka = glm::vec3(aiAmbientColor.r,aiAmbientColor.g,aiAmbientColor.b);
    //std::cout<<"ka : " << ka.x << ", " << ka.y <<", " << ka.z <<std::endl;
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);
    kd = glm::vec3(aiDiffuseColor.r,aiDiffuseColor.g,aiDiffuseColor.b);

    mat->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecularColor);
    ks = glm::vec3(aiSpecularColor.r,aiSpecularColor.g,aiSpecularColor.b);

    mat->Get(AI_MATKEY_SHININESS, aiShininess);
    shininess = static_cast<float>(aiShininess);

    Material myMaterial(ka,ks,kd,shininess,name);

    internalMat.push_back(myMaterial); 

    return internalMat;
}
void Model::Info()
{
     std::cout << "Number of meshes of this model : " << m_vMesh.size() << std::endl;
     for(int i=0; i<m_vMesh.size(); i++){
        std::cout<<"Mesh : "<<i<<std::endl; 
        std::cout<<" Has : " <<m_vMesh[i]->m_vertex.size()<< " vertices"<<std::endl; 
        std::cout<<" Has : " <<m_vMesh[i]->m_index.size()<< " indices"<<std::endl; 
        
     }
}

void Model::setDirPath(const std::string &dirPath)
{
    m_dirPath = dirPath;   
}
Shader &Model::getShader()
{
    return m_shader;
}
const std::string &Model::getDirPath() const
{
    // TODO: insert return statement here
    return m_dirPath;
}
