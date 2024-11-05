#if !defined(_ANIMATION_H_)
#define _ANIMATION_H_
#include "../AnimatedModel/animated_model.hpp"
class Animation : public Renderable { 

    private:
        std::vector<glm::mat4> m_finalBoneMat;
        float m_currentTime; 
        float m_deltaTime;
        std::shared_ptr<AnimatedModel> m_aModel;  //animated model 
        int m_numOfAnim;
        int m_ticksPerSecond;  
        float m_Duration;
        const aiScene *pScene;
        glm::mat4 m_ModelMatrix; 
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;   
    public:
        Animation();
        
        Animation( std::shared_ptr<AnimatedModel> animMode);
        ~Animation();
        void boneTransform();
        void computeBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
        void performBoneTransform(const AssimpNodeData *node, fungl::Matrix4f parentTransform);
        void load(const std::string &path);
        void load(const ModelPaths &paths);
        std::vector<glm::mat4> getFinalBoneMatrices();
        void play(std::shared_ptr<AnimatedModel> animMode);
        void updateTime(float deltaTime) override; 
        void create(std::shared_ptr<AnimatedModel> animMode);
        void display(Shader &shader);
        void setModelMAtrix();
        std::shared_ptr<AnimatedModel> getAnimatedModel();

        //Implementations of the base class
        
        void draw() override;
        Shader& getShader() override ; 
        glm::mat4 getViewMatrix() override ;
        void setViewMatrix(const glm::mat4 &viewMatrix) override; 
        glm::mat4 getProjectionMatrix() override;

        //FActory function

        static std::shared_ptr<Animation> create(){
            return std::make_shared<Animation>();
        }

};

#endif // _ANIMATION_H_
