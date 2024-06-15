#if !defined(_ANIMATION_H_)
#define _ANIMATION_H_
#include "../AnimatedModel/animated_model.hpp"
class Animation{

    private:
        std::vector<glm::mat4> m_finalBoneMat;
        std::shared_ptr<AnimatedModel> m_currentAnimation;
        float m_currentTime; 
        float m_deltaTime;  
    public:
        Animation();
        Animation( std::shared_ptr<AnimatedModel> animMode);
        ~Animation();

        void computeBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
        std::vector<glm::mat4> getFinalBoneMatrices();
        void play(std::shared_ptr<AnimatedModel> animMode);
        void update(float deltaTime); 
        void create(std::shared_ptr<AnimatedModel> animMode);  



};

#endif // _ANIMATION_H_
