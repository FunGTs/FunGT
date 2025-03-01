#include "bone.hpp"

Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel)
: m_bName{name}, m_bID{id}, m_bLocaleTrans{1.0f}{
   //std::cout<<"BONE CONSTRUCTOR" <<std::endl;
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
        inputData.m_position = aiPos;
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
        inputData.m_rotation = aiOr; 
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
        inputData.m_scale = aiScales;
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
    return 0;
}

int Bone::getRotIndex(float animTime){
    /* Gets  the current index  on the keyRotations to
       interpolate based in the current animation time */
    for(int index=0; index<m_numRot-1; index++){
        if(animTime < m_bRotation[index+1].m_timeStamp){
            return index; 
        }
    }
    return 0;
}

int Bone::getScaleIndex(float animTime){
    /* Gets  the current index  on the keyScales to
       interpolate based in the current animation time */
    for(int index=0; index<m_numScale-1; index++){
        if(animTime < m_bScale[index+1].m_timeStamp){
            return index; 
        }
    }

    return 0;
}

void Bone::update(float animTime){
    
    glm::mat4 position = interpolatePos(animTime); 
    glm::mat4 rotation = interpolateRot(animTime);
    glm::mat4 scale = interpolateScale(animTime);

    m_bLocaleTrans = position*rotation*scale;
}

void Bone::computeLocalTransforms(float animTime)
{
    //std::cout<<"Computing Bone Transfornations at : "<<animTime<<std::endl;
    aiVector3D pos   = computeInterpolatedPosition(animTime);
    aiVector3D scale = computeInterpolatedScale(animTime);
    aiQuaternion rot = computeInterpolatedRotation(animTime);

    fungl::Matrix4f posMatrix; 
    fungl::Matrix4f scaleMatrix;

    posMatrix.transformTranslation(pos.x,pos.y,pos.z);
    scaleMatrix.scaleTransform(scale.x,scale.y,scale.z);

    fungl::Matrix4f rotMat(rot.GetMatrix());

    m_BoneLocalTransformsMat = posMatrix*rotMat*scaleMatrix;
}

fungl::Matrix4f Bone::getBoneLocalTransformMat()
{
    return m_BoneLocalTransformsMat;
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
    // if(m_numPos == 1){
    //     return glm::translate(glm::mat4(1.f),m_bPosition[0].m_position);
    // }
    
    // int p0Index = getPosIndex(animTime);
  
    // int p1Index = p0Index + 1;
    // float scaleFactor = getScaleFactor(m_bPosition[p0Index].m_timeStamp,m_bPosition[p1Index].m_timeStamp,animTime);
    // glm::vec3 finalPos = glm::mix(m_bPosition[p0Index].m_position,m_bPosition[p1Index].m_position,scaleFactor); 

    return glm::translate(glm::mat4(1.0f),glm::vec3(1.0f));
    // int nextIndex = (index + 1);
    // float deltaTime = m_bPosition[nextIndex].m_timeStamp - m_bPosition[index].m_timeStamp;
    // float factor = (animTime - m_bPosition[index].m_timeStamp) / deltaTime;
    // glm::vec3 start = m_bPosition[index].m_position;
    // glm::vec3 end = m_bPosition[nextIndex].m_position;
    // glm::vec3 delta = end - start;
    //return glm::translate(glm::mat4(1.0f),start + factor * delta);
}

glm::mat4 Bone::interpolateRot(float animTime){

    // if(m_numRot == 1){
    //     glm::quat  rot = glm::normalize(m_bRotation[0].m_orientation);
    //     return glm::toMat4(rot);
    // }
    // int p0Index = getRotIndex(animTime);
    // int p1Index = p0Index + 1; 

    // float scaleFactor = getScaleFactor(m_bRotation[p0Index].m_timeStamp,m_bRotation[p1Index].m_timeStamp,animTime);
    // glm::quat finalRot = glm::slerp(m_bRotation[p0Index].m_orientation,m_bRotation[p1Index].m_orientation,scaleFactor); 
    // finalRot = glm::normalize(finalRot); 
    return glm::mat4(1.0f);
}

glm::mat4 Bone::interpolateScale(float animTime){

    // if(m_numScale == 1){
    //     return glm::scale(glm::mat4(1.0f),m_bScale[0].m_scale); 
    // }
    // int p0Index = getScaleIndex(animTime);
    // int p1Index = p0Index + 1; 

    // float scaleFactor = getScaleFactor(m_bScale[p0Index].m_timeStamp,m_bScale[p1Index].m_timeStamp,animTime);
    // glm::vec3 finalScale = glm::mix(m_bScale[p0Index].m_scale,m_bScale[p1Index].m_scale,scaleFactor); 

    //return glm::translate(glm::mat4(1.0f),finalScale);

    return glm::mat4(1.f);
}

aiVector3D Bone::computeInterpolatedPosition(float animTime)
{
    aiVector3D outPut; 
    if(m_numPos==1){
        outPut = m_bPosition[0].m_position;
        return outPut;
    }
    int p0Index = getPosIndex(animTime);
    int p1Index = p0Index + 1;

    float t1 = m_bPosition[p0Index].m_timeStamp;
    float t2 = m_bPosition[p1Index].m_timeStamp;
    float deltaTime = t2-t1;
    float factor = (animTime - t1)/deltaTime;

    aiVector3D start =  m_bPosition[p0Index].m_position; 
    aiVector3D end = m_bPosition[p1Index].m_position; 
    aiVector3D deltaPos =  end - start;
    
    outPut = start + factor*deltaPos;
   
    return outPut;
}

aiQuaternion Bone::computeInterpolatedRotation(float animTime)
{
    aiQuaternion outPut;
     if(m_numRot == 1){
        
        return m_bRotation[0].m_rotation;
    }

    int p0Index = getRotIndex(animTime);
    int p1Index = p0Index + 1; 

    float t1 = m_bRotation[p0Index].m_timeStamp;
    float t2 = m_bRotation[p1Index].m_timeStamp;

    float deltaTime = t2-t1;
    float factor = (animTime - t1)/deltaTime;


    aiQuaternion start = m_bRotation[p0Index].m_rotation;
    aiQuaternion end = m_bRotation[p1Index].m_rotation;

    aiQuaternion::Interpolate(outPut,start,end,factor);



    return outPut.Normalize();
}

aiVector3D Bone::computeInterpolatedScale(float animTime)
{
    aiVector3D outPut;
    if(m_numScale == 1){
        return m_bScale[0].m_scale; 
    }
    int p0Index = getScaleIndex(animTime);
    int p1Index = p0Index + 1; 

    float t1 = m_bScale[p0Index].m_timeStamp; 
    float t2 = m_bScale[p1Index].m_timeStamp; 

    float deltaTime = t2 - t1; 
    float factor = (animTime - t1)/deltaTime;

    aiVector3D start = m_bScale[p0Index].m_scale;
    aiVector3D end = m_bScale[p1Index].m_scale;
    aiVector3D deltaScale = end - start;

    outPut = start + factor*deltaScale;


    return outPut;
}
