#if !defined(_ANIN_MODEL_H_)
#define _ANIN_MODEL_H_
#include "../Helpers/helpers.hpp"
#include "../Model/model.hpp"
#include <map>
struct BoneInfo{

    int m_id;
    glm::mat4 m_offset; 

};


class AnimatedModel  : public Model{

public:
    AnimatedModel();
    ~AnimatedModel();

private:
    std::map<std::string,BoneInfo> m_mapBoneInfo;
    int m_boneCounter = 0; 


    std::map<std::string,BoneInfo> &getBoneInfoMap();
    int& getBoneCount();  

    //Mesh process: 
    std::unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene) override;
    std::vector<funGTVERTEX> getVertices(aiMesh *mesh, const aiScene *scene) override;
    void extractBoneWeights(std::vector<funGTVERTEX> &vertices, aiMesh* mesh, const aiScene* scene);
    void setVertexBoneData(funGTVERTEX &vertex, int boneID, float weight);
    void setVertexBoneData(funGTVERTEX &vertex); 



};

#endif // _ANIN_MODEL_H_
