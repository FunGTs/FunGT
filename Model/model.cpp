#include "model.hpp"

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

void Model::draw()
{
    
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
     
     std::cout<< " Dir : "<< m_dirPath << std::endl; 
    processNodes(pScene->mRootNode, pScene);
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

std::vector<Texture> Model::getTextures(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Texture> textures; 
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseM  = loadMaterials(material,aiTextureType_DIFFUSE,"texture_diffuse");
    textures.insert(textures.end(),diffuseM.begin(),diffuseM.end());


    std::vector<Texture> specularM  = loadMaterials(material,aiTextureType_SPECULAR,"texture_specular");
    textures.insert(textures.end(),specularM.begin(),specularM.end());



    return std::vector<Texture>();
}

void Model::processNodes(aiNode * node, const aiScene *scene){
    //Recursive function 
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

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<funGTVERTEX> vertices; 
    std::vector<unsigned int> indices; 
    std::vector<Texture> texture; //Texture is a class
   
    vertices = getVertices(mesh, scene); 
    indices = getIndices(mesh,scene);
    texture  = getTextures(mesh,scene);
   
    return Mesh(vertices,indices,texture);
}

std::vector<Texture> Model::loadMaterials(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i<mat->GetTextureCount(type); i++){
        aiString str; 
        mat->GetTexture(type,i,&str);
        std::string txt_path = m_dirPath + "/" +  str.C_Str(); 
        //Here  we populate or texture
        Texture internalTexture;
        //internalTexture.genTexture(txt_path);
        // textures.push_back(internalTexture); 
        std::cout<<" Texture path : " << txt_path <<std::endl; 
    } 
    return textures;
}

void Model::Info()
{
     std::cout << "Number of meshes of this model : " << m_vMesh.size() << std::endl;
     for(int i=0; i<m_vMesh.size(); i++){
        std::cout<<"Mesh : "<<i<<std::endl; 
        std::cout<<" Has : " <<m_vMesh[i].m_vertex.size()<< " vertices"<<std::endl; 
        std::cout<<" Has : " <<m_vMesh[i].m_index.size()<< " indices"<<std::endl; 
        
     }
}
