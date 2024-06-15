#include "bone.hpp"

Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel)
: m_bName{name}, m_bID{id}, m_bLocaleTrans{1.0f}{
   std::cout<<"BONE CONSTRUCTOR" <<std::endl;
   setBonePos(channel);
   setBoneRot(channel); 
   setBoneScale(channel); 
}
Bone::~Bone(){
}
void Bone::setBonePos(const aiNodeAnim *channel)
{

    m_numPos = channel->mNumPositionKeys;
    for(int i=0; i<m_numPos; ++i){

        aiVector3D aiPos = channel->mPositionKeys[i].mValue; 
        float time = channel->mPositionKeys[i].mTime; 
        KeyPosition inputData;
        inputData.m_position = funGL::Helpers::gtGLMVec(aiPos);
        inputData.m_timeStamp = time; 
        m_bPosition.push_back(inputData);

    }
}

void Bone::setBoneRot(const aiNodeAnim *channel) {

    m_numRot = channel->mNumRotationKeys; 
    for(int i = 0; i<m_numRot; i++){

        aiQuaternion aiOr = channel-> mRotationKeys[i].mValue; 
        float time = channel->mRotationKeys[i].mTime; 
        KeyRotation inputData; 
        inputData.m_orientation = funGL::Helpers::getGLMQuat(aiOr); 
        inputData.m_timeStamp = time; 
        m_bRotation.push_back(inputData); 

    }
}

void Bone::setBoneScale(const aiNodeAnim *channel){

    m_numScale = channel->mNumScalingKeys; 
    for(int i = 0; i<m_numScale; i++){

        aiVector3D aiScales = channel->mScalingKeys[i].mValue; 
        float time = channel->mScalingKeys[i].mTime; 
        KeyScale inputData; 
        inputData.m_scale = funGL::Helpers::gtGLMVec(aiScales);
        inputData.m_timeStamp = time; 
        m_bScale.push_back(inputData);  


    }
}

glm::mat4 Bone::getLocalTransform(){
    return m_bLocaleTrans;
}

std::string Bone::getBoneName(){
    return m_bName;
}

int Bone::getBoneID(){
    return m_bID;
}

int Bone::getPosIndex(float animTime){
    /* Gets  the current index  on the keyPositions to
       interpolate based in the current animation time */
    for(int index=0; index<m_numPos-1; index++){
        if(animTime < m_bPosition[index+1].m_timeStamp){
            return index; 
        }
    }
    return -1;
}

int Bone::getRotIndex(float animTime){
    /* Gets  the current index  on the keyRotations to
       interpolate based in the current animation time */
    for(int index=0; index<m_numRot-1; index++){
        if(animTime < m_bRotation[index+1].m_timeStamp){
            return index; 
        }
    }
    return -1;
}

int Bone::getScaleIndex(float animTime){
    /* Gets  the current index  on the keyScales to
       interpolate based in the current animation time */
    for(int index=0; index<m_numScale-1; index++){
        if(animTime < m_bScale[index+1].m_timeStamp){
            return index; 
        }
    }

    return -1;
}

void Bone::update(float animTime){
    
    glm::mat4 position = interpolatePos(animTime); 
    glm::mat4 rotation = interpolateRot(animTime);
    glm::mat4 scale = interpolateScale(animTime);

    m_bLocaleTrans = position*rotation*scale;
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime){
     /* Gets normalized value for Lerp:linear interpolation & Slerp: spherical interpolation*/
    float scaleFactor = 0.0f;  

    float midWayLength = animTime - lastTimeStamp; 
    float framesDiff = nextTimeStamp - lastTimeStamp; 

    scaleFactor = midWayLength/framesDiff; 
    
    return scaleFactor;
}

glm::mat4 Bone::interpolatePos(float animTime)
{
    /*figures out which position keys to interpolate b/w and performs the interpolation 
    and returns the translation matrix*/
    if(m_numPos == 1){
        return glm::translate(glm::mat4(1.f),m_bPosition[0].m_position);
    }
    
    int p0Index = getPosIndex(animTime);
  
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(m_bPosition[p0Index].m_timeStamp,m_bPosition[p1Index].m_timeStamp,animTime);
    glm::vec3 finalPos = glm::mix(m_bPosition[p0Index].m_position,m_bPosition[p1Index].m_position,scaleFactor); 

    return glm::translate(glm::mat4(1.0f),finalPos);
}

glm::mat4 Bone::interpolateRot(float animTime){

    if(m_numRot == 1){
        glm::quat  rot = glm::normalize(m_bRotation[0].m_orientation);
        return glm::toMat4(rot);
    }
    int p0Index = getRotIndex(animTime);
    int p1Index = p0Index + 1; 

    float scaleFactor = getScaleFactor(m_bRotation[p0Index].m_timeStamp,m_bRotation[p1Index].m_timeStamp,animTime);
    glm::quat finalRot = glm::slerp(m_bRotation[p0Index].m_orientation,m_bRotation[p1Index].m_orientation,scaleFactor); 
    finalRot = glm::normalize(finalRot); 
    return glm::toMat4(finalRot);
}

glm::mat4 Bone::interpolateScale(float animTime){

    if(m_numScale == 1){
        return glm::scale(glm::mat4(1.0f),m_bScale[0].m_scale); 
    }
    int p0Index = getScaleIndex(animTime);
    int p1Index = p0Index + 1; 

    float scaleFactor = getScaleFactor(m_bScale[p0Index].m_timeStamp,m_bScale[p1Index].m_timeStamp,animTime);
    glm::vec3 finalScale = glm::mix(m_bScale[p0Index].m_scale,m_bScale[p1Index].m_scale,scaleFactor); 

    return glm::translate(glm::mat4(1.0f),finalScale);
}
