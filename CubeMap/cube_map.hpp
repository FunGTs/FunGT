#if !defined(_CUBE_MAP_H_)
#define _CUBE_MAP_H_
#include <memory>
#include "../Geometries/primitives.hpp"
#include "../Shaders/shader.hpp"
#include "../Renderable/renderable.hpp"
#include "../DataPaths/datapaths.hpp"
#include "../GraphicsRenderBackend/gpu_buffer.hpp"
class CubeMap : public Renderable {

    private:
        std::vector<PrimitiveVertex> m_vertex;
        std::vector<uint32_t>        m_index;
        glm::mat4 m_viewMatrix = glm::mat4(1.f);
        glm::mat4 m_projectionMatrix = glm::mat4(1.f);
        std::unique_ptr<GPUBuffer> m_buffer;
    public:
        Texture texture;
        std::unique_ptr<Shader> shader;


    public:
        CubeMap();
        CubeMap(glm::vec3 cubePos);
        CubeMap(float xpos, float ypos, float zpos); 
        ~CubeMap();

        void build(const std::vector<std::string> &pathVec);
        void setData();
        void set();
        
        void setVertices(const PrimitiveVertex *vertices, const unsigned numOfvert);
        void setIndices(const uint32_t *indices, const unsigned numOfindices);
        PrimitiveVertex *getVertices();
        uint32_t* getIndices();
        unsigned getNumOfVertices();
        unsigned getNumOfIndices();
        void setShaders(std::string vs, std::string fs);
       
        //void setViewMatrix(const glm::mat4 &viewMatrix);
        void setProjectionMatrix(const glm::mat4 &projectionMatrix);

        //Factory functions

        static std::shared_ptr<CubeMap> create(){
            return std::make_shared<CubeMap>();
        }


        void addData(const ModelPaths &data);

        //Definitions from the base class

        void draw() override;
        glm::mat4 getViewMatrix() override;
        glm::mat4 getProjectionMatrix() override;
        Shader &getShader() override;
        void setViewMatrix(const glm::mat4 &viewMatrix) override;
        void enableDepthFunc() override; 
        void disableDepthFunc() override;   

        


};

#endif // _CUBE_MAP_H_
