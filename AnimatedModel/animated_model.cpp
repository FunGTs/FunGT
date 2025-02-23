#include "animated_model.hpp"


AnimatedModel::AnimatedModel()
: Model(){
    std::cout<<"Animated Model Default Constructor"<<std::endl;
    m_filePath = "";
}

AnimatedModel::~AnimatedModel(){

    std::cout<<"Animated Model Default Destructor"<<std::endl;

}

void AnimatedModel::loadModel(const std::string &path){
   m_filePath = path;
    /*std::cout << "Assimp Version: "
              << aiGetVersionMajor() << "."
              << aiGetVersionMinor() << "."
              << aiGetVersionRevision() << std::endl;*/
    std::cout <<"Loading an Animated Model "<<std::endl; 
    Assimp::Importer import; 
    const aiScene *pScene = import.ReadFile(path, ASSIMP_LOAD_FLAGS); 
    if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode){
        
        std::cout<<"ERROR in ASSIMP :"<<import.GetErrorString()<<std::endl;
        exit(0);
        //return;  
    }
    auto anim = pScene->mAnimations[0];
    m_numOfAnim = pScene->mNumAnimations;
    m_Duration = pScene->mAnimations[0]->mDuration;
    m_ticksPerSecond = pScene->mAnimations[0]->mTicksPerSecond;
    //aiMatrix4x4 globalTransform = pScene->mRootNode->mTransformation; 
    std::cout<<"Number of animations : " << m_numOfAnim  <<std::endl; 
    std::cout<<"Duration             : " <<m_Duration << std::endl; 
    std::cout<<"Ticks per second     : "<<m_ticksPerSecond<<std::endl; 
    m_dirPath = path.substr(0, path.find_last_of('/'));
    
    //Process the Nodes: 
     
    //std::cout<< " Dir : "<< m_dirPath << std::endl; 
    //processNodes(pScene->mRootNode, pScene);

    processAssimpScene(pScene->mRootNode, pScene);

    readHeirarchyData(m_rootNode,pScene->mRootNode);

    boneTransform();    

    setBones(anim);

    std::cout<<"Number of Bones : "<<m_bones.size()<<std::endl; 

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

int AnimatedModel::getTicksPerSecond(){
    return m_ticksPerSecond;
}

float AnimatedModel::getDuration(){
    return m_Duration;
}

AssimpNodeData &AnimatedModel::getRootNode()
{
    return m_rootNode; 
}

void AnimatedModel::readHeirarchyData(AssimpNodeData &dest, const aiNode *source){

    dest.name = source->mName.data; 
    dest.transform = fungl::Matrix4f(source->mTransformation);
    std::cout<<"Source Mat : "<<dest.name<<std::endl;
    funGL::Helpers::printAiMatrix4x4(source->mTransformation);
    dest.childrenCount = source->mNumChildren; 
    std::cout<<"Node Name : "<<dest.name<<std::endl;
    std::cout<<"After convert  :"<<std::endl; 
    dest.transform.print();
    for(int i = 0; i<source->mNumChildren; i++){
        AssimpNodeData newData; 
        readHeirarchyData(newData,source->mChildren[i]);
        dest.children.push_back(newData); 
    }
}

void AnimatedModel::setBones(aiAnimation *animation){

    std::cout<<"Setting the Bones " <<std::endl;
    int size = animation->mNumChannels; //Bone animation channels

    //reading channels (bones engaged in an animation and their keyframes)
    for(int i = 0; i<size; i++){
        aiNodeAnim* channel = animation->mChannels[i]; 
        std::string boneName = channel->mNodeName.data;

        //Checks if there is a missing bone.
        if( m_mapBoneInfo.find(boneName) == m_mapBoneInfo.end()  ){
            std::cout<<"Adding Missing Bone "<<std::endl; 
            m_mapBoneInfo[boneName].m_id = m_boneCounter; 
            m_boneCounter++; 

        }
        Bone inBone{channel->mNodeName.data,m_mapBoneInfo[channel->mNodeName.data].m_id,channel}; 
        std::cout<<"Bone : "<< channel->mNodeName.data <<" added. "<<std::endl;
        m_bones.push_back(inBone);
    }
    
    

}

std::unique_ptr<Bone> AnimatedModel::findBone(const std::string &name){

    for( auto& item : m_bones){
        if(item.getBoneName() == name){
            return std::make_unique<Bone>(item);
        }
    }
    return nullptr;
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
    return std::make_unique<Mesh>(vertices,indices,texture);
}

void AnimatedModel::printGlmMat4(glm::mat4 &mat)
{
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
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
    std::cout<<"***Extracting Bone Weights***"<<std::endl;
    for(int index = 0;  index<mesh->mNumBones; index++){
        int id = -1; 
        std::string boneName = mesh->mBones[index]->mName.C_Str();
        if(m_mapBoneInfo.find(boneName) == m_mapBoneInfo.end()){
            //the bone is not in the map
            std::cout<<"Adding new bone info : "<<boneName<<std::endl; 
            BoneInfo bone_info; 
            bone_info.m_id = m_boneCounter;
            //bone_info.m_offset = funGL::Helpers::convertMatToGlm(mesh->mBones[index]->mOffsetMatrix);
            bone_info.m_offset = fungl::Matrix4f(mesh->mBones[index]->mOffsetMatrix);
            m_mapBoneInfo[boneName] = bone_info;   //inserts to map
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
        //std::cout<<"This bone is influenced by : "<< nWeights << " weights" <<std::endl; 
        for(int i = 0; i<nWeights; i++){
            int vertexId = weights[i].mVertexId;
            float currWeight = weights[index].mWeight;
            
            setVertexBoneData(vertices[vertexId],id,currWeight);


        } 

    }
}

void AnimatedModel::setVertexBoneData(funGTVERTEX &vertex, int boneID, float weight){

   

    // Ensure weight is not a tiny floating-point error value
    weight = (std::abs(weight) < EPSILON) ? 0.0f : weight;

    for (int i = 0; i <maxBoneInfluencePerVertex; i++) {
        if (vertex.m_BoneIDs[i] == boneID) {
            // Bone already present, do nothing
            return;
        }
    }
    for(int i=0; i<maxBoneInfluencePerVertex; i++){
        if(vertex.m_BoneIDs[i]<0){
            vertex.m_BoneIDs[i] = boneID;
            vertex.m_Weights[i] = weight;
            return; 
        }
    }
    std::cout << "Number of vertices per bone influence exceeded !!" << std::endl;

    // If we reach here, all slots are filled
    // Replace the smallest weight if the new weight is larger
    int maxIndex = 0;
    for (int i = 1; i < maxBoneInfluencePerVertex; i++) {
        if (vertex.m_Weights[i] > vertex.m_Weights[maxIndex]) {
            maxIndex = i;
        }
    }

    if (weight + EPSILON < vertex.m_Weights[maxIndex]) {
        std::cout << "** Replacing bone ID " << vertex.m_BoneIDs[maxIndex] << " with bone ID " << boneID << " due to higher weight ***" << std::endl;
        vertex.m_BoneIDs[maxIndex] = boneID;
        vertex.m_Weights[maxIndex] = weight;
    } else {
        std::cout << "** New weight " << weight << " is not larger than the current minimum weight " << vertex.m_Weights[maxIndex] << " ***" << std::endl;
    }
  
}

void AnimatedModel::setVertexBoneData(funGTVERTEX &vertex){
    for(int i = 0; i<maxBoneInfluencePerVertex; i++){
        vertex.m_BoneIDs[i] = -1; 
        vertex.m_Weights[i] = 0.0f; 
    }
}

void AnimatedModel::computeBoneTransform(const AssimpNodeData *node, fungl::Matrix4f parentTransform)
{
    std::cout << " **** Computing Bone Transformations ****" << std::endl;
    std::string nodeName = node->name;
    //fungl::Matrix4f nodeTransform(node->transform);
    std::cout << "Node Name : " << nodeName << std::endl;


    fungl::Matrix4f globalTransform =  parentTransform*node->transform;
    //printf("Parent Transform : \n");    
    //parentTransform.print();
    //printf("Node Transform : \n");    
    //node->transform.print();
    //printf("Global Transform : \n");    
    //globalTransform.print();

    if (m_mapBoneInfo.find(nodeName) != m_mapBoneInfo.end())
    {
        int index = m_mapBoneInfo[nodeName].m_id;
        fungl::Matrix4f offsetMat(m_mapBoneInfo[nodeName].m_offset);
        //printf("Offset Matrix : \n");
        //offsetMat.print();
        //std::cout << "m_finalBoneMat.size() : " << m_finalBoneMat.size() << std::endl;
        if (index >= this->m_finalBoneMat.size())
        {
            m_finalBoneMat.resize(index + 1, glm::mat4(1.0f));
        }
        fungl::Matrix4f finalMatrix;
        finalMatrix = globalTransform * offsetMat;
        //printf("Final Bone Matrix for index: %d \n", index); 
        //finalMatrix.print(); 
        glm::mat4 glmMatFinal = fungl::Matrix4fToGlmMat4(finalMatrix);
        m_finalBoneMat[index] = glmMatFinal;
        //printf("Final glm::Bone Matrix for index: %d \n", index);   
        //printGlmMat4(glmMatFinal);
        
        
    }
    for (int i = 0; i < node->childrenCount; i++)
    {
        computeBoneTransform(&node->children[i], globalTransform);
    }
}

void AnimatedModel::boneTransform()
{
    std::cout << "Bone Transform" << std::endl;
    fungl::Matrix4f matId;
    matId.identity();
    //matId.print();  
    computeBoneTransform(&m_rootNode, matId);
}

std::vector<glm::mat4> AnimatedModel::getFinalBoneMatrices()
{   
    return m_finalBoneMat;
}

std::string AnimatedModel::getFilePath()
{
    return m_filePath;
}
