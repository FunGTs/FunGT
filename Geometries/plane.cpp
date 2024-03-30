#include "plane.hpp"

Plane::Plane()
: Primitive(){
    printf("Plane default constructor\n");
}
Plane::Plane(glm::vec3 planePos)
: Primitive(planePos){
    m_ShapeRot.x = 75;
}
Plane::Plane(float xpos, float ypos, float zpos)
: Primitive(xpos,ypos,zpos){
    printf("Plane constructor: pos params\n");
    
    m_ShapeRot.x = -70;
}
Plane::~Plane()
{
    printf("USING Plane DESTRUCTOR\n");

}

void Plane::setScale(glm::vec3 scale)
{
    m_ShapeScale = scale; 

}

void Plane::create(const std::string &pathToTexture)
{
    this->setData();
	std::cout<<"Plane Create function : "<<std::endl;

    std::cout<<"vertices : "<< this->getNumOfVertices()<<std::endl;

	vao.genVAO();
    vertexBuffer.genVB(this->getVertices(),this->sizeOfVertices());
    
    
    this->setAttribs();


	texture.genTexture(pathToTexture);
    texture.active();
    texture.bind();

    //All binded above must be released
    vao.unbind();
    this->unsetAttribs();
    vertexBuffer.release(); 

    std::cout<<"End Plane Create function : "<<std::endl;

}

void Plane::draw()
{
    texture.active();
    texture.bind();
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->getNumOfVertices());
}

void Plane::setData()
{
    //     Vertex vertices[] = {
    //     // Positions          // Texture coordinates
    //     glm::vec3(1.0f, 0.0f, 1.0f),     glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f), // Top-right
    //     glm::vec3(1.0f, 0.0f, -1.0f),    glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f), // Bottom-right
    //     glm::vec3(-1.0f, 0.0f, -1.0f),   glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f), // Bottom-left
    //     glm::vec3(-1.0f, 0.0f, 1.0f),   glm::vec3(0.f, 0.f, 0.f), glm::vec2( 0.0f, 0.0f)  // Top-left
    // };
    Vertex vertices[] = {
    // Positions             // Texture Coords
    glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 1.0f) ,
    glm::vec3(-1.0f, -1.0f, 0.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),

    glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 1.0f) ,
    glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f) ,
     glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 1.0f) 
};
    unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
    // GLuint indices[] = {
    //     0, 1, 3, // First triangle
    //     1, 2, 3  // Second triangle
    // };
        GLuint indices[] = {

        0, 1, 2,
        0 , 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices);
}
glm::mat4 Plane::getModelMatrix()
{
    return this->m_ShapeModelMatrix;
}

void Plane::setPosition(glm::vec3 pos)
{

    m_ShapePos = pos; 

}

void Plane::setModelMatrix()
{
    //std::cout<<"pos : "<<m_ShapePos.x << ", " <<m_ShapePos.y<<", "<<m_ShapePos.z<<std::endl; 
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
    
}

void Plane::updateModelMatrix(float zrot)
{
   
    m_ShapeModelMatrix = glm::mat4(1.f); 
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
}