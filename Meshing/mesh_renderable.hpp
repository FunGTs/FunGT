#if !defined(_MESH_RENDERABLE_HPP_)
#define _MESH_RENDERABLE_HPP_

#include <memory>

#include "Renderable/renderable.hpp"
#include "mesh_geometry.hpp"
#include "Material/material.hpp"




class MeshRenderable : public Renderable{

    private: 

        std::shared_ptr<MeshGeometry> m_meshgeom;
        std::unique_ptr<Shader> m_shader; 

        glm::mat4 m_modelMatrix = glm::mat4(1.0f);
        glm::mat4 m_viewMatrix = glm::mat4(1.0f);
        glm::vec3 m_position = glm::vec3(0.0f);
        glm::vec3 m_rotation = glm::vec3(0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f);


    public:
        explicit MeshRenderable(std::shared_ptr<MeshGeometry> meshgeom);

        void load();
        void position(float x = 0.0f, float y = 0.0f, float z = 0.0f);
        void rotation(float x = 0.0f, float y = 0.0f, float z = 0.0f);
        void scale(float s = 1.0f);
        void scale(float x, float y, float z);
        glm::vec3 getPosition() const;
        glm::vec3 getRotation() const;
        glm::vec3 getScale() const;
        std::shared_ptr<MeshGeometry> getMeshGeometry() const;
        void draw() override;
        Shader& getShader() override;
        glm::mat4 getModelMatrix() const override;
        void setViewMatrix(const glm::mat4& viewMatrix) override;
        void updateModelMatrix() override;


};

#endif // _MESH_RENDERABLE_HPP_
