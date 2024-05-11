#include "animated_model.hpp"


AnimatedModel::AnimatedModel()
: Model(){
    std::cout<<"Animated Model Default Constructor"<<std::endl;
}

AnimatedModel::~AnimatedModel(){

    std::cout<<"Animated Model Default Destructor"<<std::endl;

}

std::map<std::string, BoneInfo> &AnimatedModel::getBoneInfoMap()
{
    // TODO: insert return statement here

    return m_mapBoneInfo; 
}

int &AnimatedModel::getBoneCount()
{
    // TODO: insert return statement here
    return m_boneCounter; 
}

std::unique_ptr<Mesh> AnimatedModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
    
    std::vector<funGTVERTEX> vertices; 
    std::vector<unsigned int> indices; 
    std::vector<Texture > texture; //Texture is a class
    std::vector<Material> materials; 

    vertices = getVertices(mesh, scene); 
    indices = getIndices(mesh,scene);
    texture  = getTextures(mesh,scene);
    materials = getMaterials(mesh, scene);
    std::cout<<"Indices loaded   : "<<indices.size()<<std::endl; 
    std::cout<<"Vertices loaded  : "<<vertices.size()<<std::endl; 
    std::cout<<"Textures loaded  : "<<texture.size()<<std::endl; 
    std::cout<<"Materials loaded : "<<materials.size()<<std::endl; 
    if(texture.size()==0 && materials.size()>0){
         std::cout<<"Mesh with only material"<<std::endl; 
        return std::make_unique<Mesh>(vertices,indices,materials);
    }
    std::cout<<"Hi"<<std::endl; 
    return std::make_unique<Mesh>(vertices,indices,texture);
}

std::vector<funGTVERTEX> AnimatedModel::getVertices(aiMesh *mesh, const aiScene *scene){

   std::vector<funGTVERTEX> vertices; 
    //std::cout<<"  Mesh contains:  " <<mesh->mNumVertices << " vertices"<<std::endl; 
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
       
       glm::vec3 auxVec; //to pass the assimp vertex to glm::vec3 
       auxVec.x = mesh->mVertices[i].x; 
       auxVec.y = mesh->mVertices[i].y; 
       auxVec.z = mesh->mVertices[i].z; 
       // puts the glm::vec3 inside our defined Vertex struct
       funGTVERTEX internalVertex;
       setVertexBoneData(internalVertex);  
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
    extractBoneWeights(vertices,mesh,scene);
    return vertices;
}

void AnimatedModel::extractBoneWeights(std::vector<funGTVERTEX> &vertices, aiMesh *mesh, const aiScene *scene)
{
    for(int index = 0;  index<mesh->mNumBones; index++){
        int id = -1; 
        std::string boneName = mesh->mBones[index]->mName.C_Str();
        if(m_mapBoneInfo.find(boneName) == m_mapBoneInfo.end()){
            //the bone is not in the map
            std::cout<<"Adding new bone info : "<<boneName<<std::endl; 
            BoneInfo bone_info; 
            bone_info.m_id = m_boneCounter;
            bone_info.m_offset = funGL::Helpers::convertMatToGlm(mesh->mBones[index]->mOffsetMatrix);
            m_mapBoneInfo[boneName] = bone_info;   //inserts to mape
            id = m_boneCounter; // zero at the start
            m_boneCounter++; //increases the bone counter
        }
        else{
            //The bone is in the map
            id = m_mapBoneInfo[boneName].m_id; 
        }
        aiVertexWeight *weights = mesh->mBones[index]->mWeights;
        int nWeights = mesh->mBones[index]->mNumWeights;
        //iterate to the whole weights
        for(int i = 0; i<nWeights; i++){
            int vertexId = weights[i].mVertexId;
            float currWeight = weights[index].mWeight;
            
            setVertexBoneData(vertices[vertexId],id,currWeight);


        } 

    }
}

void AnimatedModel::setVertexBoneData(funGTVERTEX &vertex, int boneID, float weight){

    for(int i=0; i<maxBoneInfulencePerVertex; i++){
        if(vertex.m_BoneIDs[i]<0){
            vertex.m_Weights[i] = weight; 
            vertex.m_BoneIDs[i] = boneID;
            break; //awfull,needs to be removed

        }

    }

}

void AnimatedModel::setVertexBoneData(funGTVERTEX &vertex){
    for(int i = 0; i<maxBoneInfulencePerVertex; i++){
        vertex.m_BoneIDs[i]=-1; 
        vertex.m_Weights[i] = 0.0f; 
    }
}
