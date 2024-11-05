#if !defined(_ANIN_MODEL_H_)
#define _ANIN_MODEL_H_
#include "../Helpers/helpers.hpp"
#include "../Model/model.hpp"
#include "../Bone/bone.hpp"
#include "../Matrix/matrix4x4f.h"
#include <map>
struct BoneInfo{

    int m_id;
    fungl::Matrix4f m_offset; 

};
struct AssimpNodeData{
    fungl::Matrix4f transform;  
    std::string name; 
    int childrenCount; 
    std::vector<AssimpNodeData> children; 
};

class AnimatedModel  : public Model{

public:
    int m_numOfAnim;
    int m_ticksPerSecond;  
    float m_Duration;
    AssimpNodeData m_rootNode; 
    std::vector<Bone> m_bones;
    std::string m_filePath;
    std::map<std::string,BoneInfo> m_mapBoneInfo;

    AnimatedModel();
    ~AnimatedModel();

    void loadModel(const std::string &path) override;
    std::unique_ptr<Bone> findBone(const std::string& name);
    std::map<std::string,BoneInfo> &getBoneInfoMap();
    int& getBoneCount();
    int getTicksPerSecond(); 
    float getDuration();
    AssimpNodeData &getRootNode();   
    std::vector<glm::mat4> getFinalBoneMatrices();
    std::string getFilePath();
    void boneTransform();
    void readHeirarchyData(AssimpNodeData &dest, const aiNode* source);
    void setBones(aiAnimation *animation); 

private:
    
    int m_boneCounter = 0;
    std::vector<glm::mat4> m_finalBoneMat; 





    void printGlmMat4(glm::mat4 &mat);

    //Mesh process: 
    std::unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene) override;
    std::vector<funGTVERTEX> getVertices(aiMesh *mesh, const aiScene *scene) override;
    void extractBoneWeights(std::vector<funGTVERTEX> &vertices, aiMesh* mesh, const aiScene* scene);
    void setVertexBoneData(funGTVERTEX &vertex, int boneID, float weight);
    void setVertexBoneData(funGTVERTEX &vertex);

    //Animation ?? 
    void computeBoneTransform(const AssimpNodeData* node, fungl::Matrix4f parentTransform);
    




};

#endif // _ANIN_MODEL_H_