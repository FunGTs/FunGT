#include "animation.hpp"

Animation::Animation(){
     std::cout<<"Animation Default Constructor"<<std::endl;
}

Animation::Animation(std::shared_ptr<AnimatedModel> animModel)
{
    std::cout<<"Animation Constructor"<<std::endl;
    m_currentAnimation = animModel;
    m_currentTime = 0.0f;
}

Animation::~Animation(){
     std::cout<<"Animation Destructor"<<std::endl;
}

void Animation::computeBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform){
    
    std::cout<<"Computing Bone Transformations"<<std::endl; 
    std::string nodeName = node->name; 
    glm::mat4 nodeTransform = node->transform;
    std::cout<<"Node Name : "<< nodeName <<std::endl; 

    std::unique_ptr<Bone> bone = m_currentAnimation->findBone(nodeName); 
    if(bone){
        std::cout<<"Bone found : " << nodeName << std::endl; 
        bone->update(m_currentTime);
        nodeTransform = bone->getLocalTransform(); 
    }
    else{
        std::cout<<"Bone not fond"<<std::endl; 
    }

    glm::mat4 globalTransform = parentTransform*nodeTransform; 
    auto boneInfoMap = m_currentAnimation->getBoneInfoMap(); 

    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].m_id; 
        glm::mat4 offsetMat = boneInfoMap[nodeName].m_offset;
        std::cout<<"m_finalBoneMat.size() : "<< m_finalBoneMat.size()<<std::endl;
        if(index >= m_finalBoneMat.size()){
            m_finalBoneMat.resize(index + 1, glm::mat4(1.0f));
        }
        m_finalBoneMat[index] = globalTransform*offsetMat; 
    }
    for(int i = 0;  i<node->childrenCount; i++){
        computeBoneTransform(&node->children[i],globalTransform); 
    }

}

std::vector<glm::mat4> Animation::getFinalBoneMatrices()
{
    return m_finalBoneMat;
}

void Animation::play(std::shared_ptr<AnimatedModel> animModel){

    m_currentAnimation = animModel; 
    m_currentTime = 0.0f; 

}

void Animation::update(float deltaTime){

    m_deltaTime = deltaTime; 
    if(m_currentAnimation){
        m_currentTime += m_currentAnimation->getTicksPerSecond()*deltaTime; 
        m_currentTime = fmod(m_currentTime,m_currentAnimation->getDuration());
        computeBoneTransform(&m_currentAnimation->getRootNode(),glm::mat4(1.0f));

    }

}

void Animation::create(std::shared_ptr<AnimatedModel> animModel){
    std::cout<<"Creating Animation"<<std::endl;
    m_currentAnimation = animModel;
    m_currentTime = 0.0f;
    std::cout<<"End of the create function"<<std::endl;
}
