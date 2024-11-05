#include "animation.hpp"

Animation::Animation()
{
    std::cout << "Animation Default Constructor" << std::endl;
    m_aModel = std::make_shared<AnimatedModel>(); 
}

Animation::Animation(std::shared_ptr<AnimatedModel> animModel)
{
    std::cout << "Animation Constructor" << std::endl;
    m_aModel = animModel;
    m_currentTime = 0.0f;
}

Animation::~Animation()
{
    std::cout << "Animation Destructor" << std::endl;
}

void Animation::boneTransform()
{
    std::cout << "Bone Transform" << std::endl;
    fungl::Matrix4f matId;
    matId.identity();
    matId.print();
    performBoneTransform(&m_aModel->m_rootNode, matId); 
}

void Animation::computeBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)
{

    std::cout << "Computing Bone Transformations" << std::endl;
    // std::string nodeName = node->name;
    // glm::mat4 nodeTransform = node->transform;
    // std::cout << "Node Name : " << nodeName << std::endl;

    // std::unique_ptr<Bone> bone = m_currentAnimation->findBone(nodeName);
    // if (bone)
    // {
    //     std::cout << "Bone found : " << nodeName << std::endl;
    //     bone->update(m_currentTime);
    //     nodeTransform = bone->getLocalTransform();
    // }
    // else
    // {
    //     std::cout << "Bone not fond" << std::endl;
    // }

    // glm::mat4 globalTransform = nodeTransform*parentTransform;
    // auto boneInfoMap = m_currentAnimation->getBoneInfoMap();

    // if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    // {
    //     int index = boneInfoMap[nodeName].m_id;
    //     glm::mat4 offsetMat = boneInfoMap[nodeName].m_offset;
    //     std::cout << "m_finalBoneMat.size() : " << m_finalBoneMat.size() << std::endl;
    //     if (index >= m_finalBoneMat.size())
    //     {
    //         m_finalBoneMat.resize(index + 1, glm::mat4(1.0f));
    //     }
    //     m_finalBoneMat[index] = globalTransform * offsetMat;
    // }
    // for (int i = 0; i < node->childrenCount; i++)
    // {
    //     computeBoneTransform(&node->children[i], globalTransform);
    // }
}

void Animation::performBoneTransform(const AssimpNodeData *node, fungl::Matrix4f parentTransform)
{
    std::cout << " **** Computing Bone Transformations :  performBoneTransform ****" << std::endl;
    std::string nodeName = node->name;
    //fungl::Matrix4f nodeTransform(node->transform);
    std::cout << "Node Name : " << nodeName << std::endl;

    fungl::Matrix4f NodeTransform(node->transform);
    
    // printf("Parent Transform : \n");    
    // parentTransform.print();
    // printf("Node Transform : \n");    
    // node->transform.print();
   


    std::unique_ptr<Bone> bone = m_aModel->findBone(nodeName);
    if (bone)
    {
        std::cout << "Bone found : " << nodeName << std::endl;
        bone->computeLocalTransforms(m_currentTime);
        NodeTransform = bone->getBoneLocalTransformMat();
    }

    fungl::Matrix4f globalTransform = parentTransform*NodeTransform;

    //printf("Global Transform : \n");    
    //globalTransform.print();

    if (m_aModel->m_mapBoneInfo.find(nodeName) != m_aModel->m_mapBoneInfo.end())
    {
        int index = m_aModel->m_mapBoneInfo[nodeName].m_id;
        fungl::Matrix4f offsetMat(m_aModel->m_mapBoneInfo[nodeName].m_offset);
        //printf("Offset Matrix : \n");
        //offsetMat.print();
        //std::cout << "m_finalBoneMat.size() : " << m_finalBoneMat.size() << std::endl;
        if (static_cast<std::size_t>(index) >= this->m_finalBoneMat.size())
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
        //funGL::Helpers::printGlmMat4(glmMatFinal);
    }
    for (int i = 0; i < node->childrenCount; i++)
    {
        performBoneTransform(&node->children[i], globalTransform);
    }
}

void Animation::load(const std::string &path)
{
    std::cout <<"Loading an Animated Model in animation "<<std::endl; 
    Assimp::Importer import; 
    pScene = import.ReadFile(path, ASSIMP_LOAD_FLAGS); 
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
    //Set the data to the Animated Model
    if(m_aModel==nullptr){
        std::cout<<"Null pointer in : m_aModel "<<std::endl;
        exit(0);
    } 
    m_aModel->setDirPath(path.substr(0, path.find_last_of('/')));
    m_aModel->processAssimpScene(pScene->mRootNode, pScene);
    m_aModel->readHeirarchyData(m_aModel->m_rootNode, pScene->mRootNode);
    m_aModel->setBones(anim);
    //m_aModel->boneTransform();
    //m_aModel->setBones(anim);
   
}

void Animation::load(const ModelPaths &data)
{
    std::cout <<"Loading an Animated Model in animation "<<std::endl; 
    Assimp::Importer import; 
    pScene = import.ReadFile(data.path, ASSIMP_LOAD_FLAGS); 
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
    //Set the data to the Animated Model
    if(m_aModel==nullptr){
        std::cout<<"Null pointer in : m_aModel "<<std::endl;
        exit(0);
    }
    m_aModel->createShader(data.vs_path, data.fs_path); 
    m_aModel->setDirPath(data.path.substr(0, data.path.find_last_of('/')));
    m_aModel->processAssimpScene(pScene->mRootNode, pScene);
    m_aModel->readHeirarchyData(m_aModel->m_rootNode, pScene->mRootNode);
    m_aModel->setBones(anim);
}

std::vector<glm::mat4> Animation::getFinalBoneMatrices()
{
    return m_finalBoneMat;
}

void Animation::play(std::shared_ptr<AnimatedModel> animModel)
{

    m_aModel = animModel;
    m_currentTime = 0.0f;
}

void Animation::updateTime(float deltaTime)
{

    m_deltaTime = deltaTime;
    if (m_aModel)
    {
        m_currentTime += m_ticksPerSecond * deltaTime;
        m_currentTime = fmod(m_currentTime, m_Duration);
        boneTransform();
        //boneTransform();
        //computeBoneTransform(&m_aModel->getRootNode(), glm::mat4(1.0f));
    }
}

void Animation::create(std::shared_ptr<AnimatedModel> animModel)
{
    std::cout << "Creating Animation" << std::endl;
    m_aModel = animModel;
    m_currentTime = 0.0f;
    std::cout << "End of the create function" << std::endl;
}

void Animation::display(Shader &shader)
{
    
    auto transforms = getFinalBoneMatrices(); 
    for(std::size_t i = 0; i<transforms.size(); i++){
            //std::cout<<"Transforms["<<i<<"] : "<<transforms[i][1][0]<<std::endl;
            funGL::Helpers::printGlmMat4(transforms[i]);
            printf("\n");
            m_aModel->getShader().setUniformMat4fv("finalBonesMatrix[" + std::to_string(i) + "]",transforms[i]);
    }
    m_aModel->draw();
}

std::shared_ptr<AnimatedModel> Animation::getAnimatedModel()
{
    return m_aModel; 
}

void Animation::draw()
{
    auto transforms = getFinalBoneMatrices(); 
    for(std::size_t i = 0; i<transforms.size(); i++){
            //std::cout<<"Transforms["<<i<<"] : "<<transforms[i][1][0]<<std::endl;
            //funGL::Helpers::printGlmMat4(transforms[i]);
            //printf("\n");
            m_aModel->getShader().setUniformMat4fv("finalBonesMatrix[" + std::to_string(i) + "]",transforms[i]);
    }
    m_aModel->draw();
}

Shader &Animation::getShader()
{
    return m_aModel->getShader(); 
}

glm::mat4 Animation::getViewMatrix()
{
    return m_ViewMatrix;
}

void Animation::setViewMatrix(const glm::mat4 &viewMatrix)
{
    m_ViewMatrix = viewMatrix; 
}

glm::mat4 Animation::getProjectionMatrix()
{
    return m_ProjectionMatrix;
}
