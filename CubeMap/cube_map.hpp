#if !defined(_CUBE_MAP_H_)
#define _CUBE_MAP_H_
#include <memory>
#include "../Geometries/primitives.hpp"
#include "../Shaders/shader.hpp"
#include "../Renderable/renderable.hpp"
#include "../DataPaths/datapaths.hpp"
class CubeMap : public Renderable {

    private:
        std::vector<PrimitiveVertex> m_vertex; 
        std::vector<GLuint> m_index;
        glm::mat4 m_viewMatrix = glm::mat4(1.f); 
        glm::mat4 m_projectionMatrix  = glm::mat4(1.f);
    public: 

        unsigned int texture_type = GL_TEXTURE_CUBE_MAP;
        VertexArrayObject m_vao; 
        VertexBuffer m_vb;
        VertexIndex m_vi; 
        Texture texture;
        Shader shader; 


    public:
        CubeMap();
        CubeMap(glm::vec3 cubePos);
        CubeMap(float xpos, float ypos, float zpos); 
        ~CubeMap();

        void build(const std::vector<std::string> &pathVec);
        void setData();
        void set();
        
        void setVertices(const PrimitiveVertex *vertices, const unsigned numOfvert);
        void setIndices(const GLuint *indices, const unsigned numOfindices);        
        PrimitiveVertex *getVertices();
        GLuint* getIndices();
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
