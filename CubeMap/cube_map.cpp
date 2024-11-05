#include "cube_map.hpp"

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
    GLuint indices[] = {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->setIndices(indices,nOfIndices);
    
}

void CubeMap::addData(const ModelPaths &data)
{
    setShaders(data.vs_path, data.fs_path);
    build(data.data_path);
    
}

void CubeMap::draw()
{    m_vao.bind();
    texture.active();
    texture.bind();
    
    glDrawElements(GL_TRIANGLES,36, GL_UNSIGNED_INT,0);
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

void CubeMap::setIndices(const GLuint *indices, const unsigned numOfindices)
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

unsigned CubeMap::getNumOfIndices()
{
    return this->m_index.size();;
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
    glDepthFunc(GL_LEQUAL);
}

void CubeMap::disableDepthFunc()
{
    glDepthFunc(GL_LESS);
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
    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
    this->setVertices(vertices,nOfvertices);
    GLuint indices[] = {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->setIndices(indices,nOfIndices);



    m_vao.genVAO(); //Generates a Vertex array object
    m_vb.genVB(); //Generates the Vertex Buffer
    m_vi.genVI(); //Generates the Vertex Buffer


    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(&vertices,sizeof(vertices));

    m_vi.bind(); 
    m_vi.indexData(indices,sizeof(indices));
    //
     //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,position));
        glEnableVertexAttribArray(0);
        //COLOR
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,normal));
        glEnableVertexAttribArray(1);
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,texcoord));
        glEnableVertexAttribArray(2);

    //Texture
    texture.genTextureCubeMap(pathVec);
    texture.active();
    texture.bind();

    //All binded above must be released
    m_vao.unbind();
    m_vi.unbind();
    // glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);
    // glDisableVertexAttribArray(2);
    // m_vb.unbind(); 



    std::cout<<"End Cube Map Create function : "<<std::endl;
}
