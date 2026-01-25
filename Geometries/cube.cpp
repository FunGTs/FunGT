#include "cube.hpp"

Cube::Cube()
: Primitive(){
    printf("USING CUBE\n");
}
Cube::Cube(glm::vec3 cubePos)
: Primitive(cubePos){
}
Cube::Cube(float xpos, float ypos, float zpos)
: Primitive(xpos,ypos,zpos){
    m_ShapeRot.x = -65;
    m_ShapeScale = glm::vec3(0.5);
}
Cube::~Cube()
{
    printf("USING CUBE DESTRUCTOR\n");

}

void Cube::create(const std::string &pathToTexture){
    std::cout<<"Cube Create function : "<<std::endl;
    this->setData();
	std::cout<<"Create function : "<<std::endl;
    std::cout<<"vertices : "<< this->getNumOfVertices()<<std::endl;

    m_vao.genVAO(); //Generates a Vertex array object
    m_vb.genVB(); //Generates the Vertex Buffer
    m_vi.genVI(); //Generates the Vertex Buffer

    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(this->getVertices(),this->sizeOfVertices());

    this->setAttribs();
	texture.genTexture(pathToTexture);
    texture.active();
    texture.bind();

    //All binded above must be released
    m_vao.unbind();
    this->unsetAttribs();
    m_vb.unbind();    
    std::cout<<"End Cube Create function : "<<std::endl;

}
void Cube::draw(){
	  //All binded above must be released

    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
        //imguiRender();
}

void Cube::setData()
{
    PrimitiveVertex vertices[] = {
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),

        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f)
    };

    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
  
    this->set(vertices,nOfvertices);
}

glm::mat4 Cube::getModelMatrix() const
{
    return this->m_ShapeModelMatrix;
}

void Cube::setPosition(glm::vec3 pos)
{

    m_ShapePos = pos; 

}

void Cube::setModelMatrix()
{
    std::cout<<"pos : "<<m_ShapePos.x << ", " <<m_ShapePos.y<<", "<<m_ShapePos.z<<std::endl; 
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
    
}

void Cube::updateModelMatrix(float zrot)
{
    m_ShapeRot.z = zrot;
    m_ShapeModelMatrix = glm::mat4(1.f); 
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
}

void Cube::setScale(glm::vec3 scale)
{
}
