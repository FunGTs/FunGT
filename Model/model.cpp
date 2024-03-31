#include "model.hpp"
#include "../vendor/stb_image/stb_image.h"
Model::Model()
{
     std::cout<<"Model Default Constructor"<<std::endl;
}

Model::Model(const std::string &path)
{

    std::cout<<"Model Constructor"<<std::endl;
    loadModel(path);
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
    std::cout<<"Model Loading"<<std::endl;
    Assimp::Importer import; 
    const aiScene *pScene = import.ReadFile(path, ASSIMP_LOAD_FLAGS); 
    if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode){
        
        std::cout<<"ERROR in ASSIMP :"<<import.GetErrorString()<<std::endl;
        return;  
    }
    m_dirPath = path.substr(0, path.find_last_of('/'));
    
    //Process the Nodes: 
     
    //std::cout<< " Dir : "<< m_dirPath << std::endl; 
    processNodes(pScene->mRootNode, pScene);
    //processAssimpScene(pScene->mRootNode, pScene);
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

       //checks if the mesh has any textures
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

 std::vector<Texture > diffuseM  = loadMaterials(material,aiTextureType_DIFFUSE,"texture_diffuse");
    textures.insert(textures.end(),diffuseM.begin(),diffuseM.end());


    //std::vector<Texture > specularM  = loadMaterials(material,aiTextureType_SPECULAR,"texture_specular");
    //textures.insert(textures.end(),specularM.begin(),specularM.end());



    return textures;
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

std::unique_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<funGTVERTEX> vertices; 
    std::vector<unsigned int> indices; 
    std::vector<Texture > texture; //Texture is a class
   
    vertices = getVertices(mesh, scene); 
    indices = getIndices(mesh,scene);
    texture  = getTextures(mesh,scene);
    std::cout<<"Indices loaded : "<<indices.size()<<std::endl; 
    std::cout<<"Vertices loaded : "<<vertices.size()<<std::endl; 
    std::cout<<"Textures loaded : "<<texture.size()<<std::endl; 
    return std::make_unique<Mesh>(vertices,indices,texture);
}

std::vector<Texture > Model::loadMaterials(aiMaterial *mat, aiTextureType type, std::string typeName)
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

            // Texture_struct  texture;
            //     texture.id = TextureFromFile(txt_path);
            //     texture.type = typeName;
            //     texture.path = txt_path;
            //     textures.push_back(texture);
            //     m_loadedTextures.push_back(texture);  // store it as texture loaded for 
        } 

        
    } 
    return textures;
}

unsigned int Model::TextureFromFile(const std::string &directory, bool gamma)
{
     stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(directory.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << directory << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// void Model::Info()
// {
//      std::cout << "Number of meshes of this model : " << m_vMesh.size() << std::endl;
//      for(int i=0; i<m_vMesh.size(); i++){
//         std::cout<<"Mesh : "<<i<<std::endl; 
//         std::cout<<" Has : " <<m_vMesh[i]->m_vertex.size()<< " vertices"<<std::endl; 
//         std::cout<<" Has : " <<m_vMesh[i]->m_index.size()<< " indices"<<std::endl; 
        
//      }
// }
