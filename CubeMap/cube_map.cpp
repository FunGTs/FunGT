#include "cube_map.hpp"
#include "../GraphicsRenderBackend/graphics_render_device.hpp"

CubeMap::CubeMap()
{
    printf("USING CUBE MAP\n");
    
}
CubeMap::CubeMap(glm::vec3 cubePos)
{
}
CubeMap::CubeMap(float xpos, float ypos, float zpos)
{
}
CubeMap::~CubeMap()
{
}

void CubeMap::setData()
{
     PrimitiveVertex vertices[] = {
        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),


        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(  -1.0f,  1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f)
    };

    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
  
    //this->set(vertices,nOfvertices);

}
void CubeMap::set()
{
    PrimitiveVertex vertices[] = 
    {
        //POSITION                         //COLOR                  //Texcoords        
        glm::vec3( -1.0f,-1.0,1.0f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),
        glm::vec3(1.0f, -1.0f, 1.0f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),
        glm::vec3(1.0f, -1.0f, -1.0f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
        glm::vec3(-1.0f, -1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),

        glm::vec3(-1.0f,  1.0f,  1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3(1.0f,  1.0f,  1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3( 1.0f,  1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3(-1.0f, 1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f)
    };
    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
    this->setVertices(vertices,nOfvertices);
    uint32_t indices[] = {
        1, 2, 6,  6, 5, 1,
        0, 4, 7,  7, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 3, 2,  2, 1, 0,
        0, 1, 5,  5, 4, 0,
        3, 7, 6,  6, 2, 3
    };
    unsigned nOfIndices = sizeof(indices) / sizeof(uint32_t);
    this->setIndices(indices, nOfIndices);
    
}

void CubeMap::addData(const ModelPaths &data)
{
    setShaders(data.vs_path, data.fs_path);
    build(data.data_path);
    
}

void CubeMap::draw() {
    texture.active();
    m_buffer->bindVAO();
    m_buffer->drawIndexed(36);
    m_buffer->unbindVAO();
}

glm::mat4 CubeMap::getViewMatrix()
{
    return m_viewMatrix;
}

glm::mat4 CubeMap::getProjectionMatrix()
{
    return m_projectionMatrix;
}

void CubeMap::setVertices(const PrimitiveVertex *vertices, const unsigned numOfvert)
{
     for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }

}

void CubeMap::setIndices(const uint32_t *indices, const unsigned numOfindices)
{
        for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->m_index.push_back(indices[i]);
    }
}

unsigned CubeMap::getNumOfVertices()
{
    return this->m_vertex.size();
}

uint32_t* CubeMap::getIndices() {
    return this->m_index.data();
}

unsigned CubeMap::getNumOfIndices()
{
    return this->m_index.size();
}

void CubeMap::setShaders(std::string vs, std::string fs)
{

    shader.create(vs,fs);
    std::cout<<"End setting shaders"<<std::endl; 
}

Shader &CubeMap::getShader()
{
    // TODO: insert return statement here
    return shader;
}

void CubeMap::setViewMatrix(const glm::mat4 &viewMatrix)
{
    
    m_viewMatrix = glm::mat4(glm::mat3(viewMatrix));
}

void CubeMap::enableDepthFunc()
{
    GraphicsRenderDevice::Get()->setDepthFunc(DepthFunc::LessEqual);
}

void CubeMap::disableDepthFunc()
{
    GraphicsRenderDevice::Get()->setDepthFunc(DepthFunc::Less);
}

void CubeMap::setProjectionMatrix(const glm::mat4 &projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
}

void CubeMap::build(const std::vector<std::string> &pathVec)
{
    std::cout<<"Cube Create function : "<<std::endl;

    PrimitiveVertex vertices[] = 
    {
        //POSITION                         //COLOR                  //Texcoords        
        glm::vec3(-1.0f,-1.0,1.0f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),
        glm::vec3(1.0f, -1.0f, 1.0f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),
        glm::vec3(1.0f, -1.0f, -1.0f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
        glm::vec3(-1.0f, -1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),

        glm::vec3(-1.0f,  1.0f,  1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3(1.0f,  1.0f,  1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3( 1.0f,  1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),
        glm::vec3(-1.0f, 1.0f, -1.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f)
    };
    unsigned nOfvertices = sizeof(vertices) / sizeof(PrimitiveVertex);
    this->setVertices(vertices, nOfvertices);

    uint32_t indices[] = {
        1, 2, 6,  6, 5, 1,
        0, 4, 7,  7, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 3, 2,  2, 1, 0,
        0, 1, 5,  5, 4, 0,
        3, 7, 6,  6, 2, 3
    };
    unsigned nOfIndices = sizeof(indices) / sizeof(uint32_t);
    this->setIndices(indices, nOfIndices);

    m_buffer = GPUBuffer::create();
    m_buffer->genVAO();
    m_buffer->bindVAO();

    m_buffer->create(BufferType::Vertex, vertices, sizeof(vertices));
    m_buffer->create(BufferType::Index,  indices,  sizeof(indices));
    m_buffer->applyFormat(PrimitiveVertex::getFormat());

    texture.genTextureCubeMap(pathVec);
    texture.active();

    m_buffer->unbindVAO();

    std::cout << "End Cube Map Create function" << std::endl;
}
