#include "fungtMesh.hpp"
funGT::Mesh::Mesh(){
    
}
funGT::Mesh::Mesh(VERTEX *vertices, unsigned int &numOfVert, GLuint *indexArray, unsigned int& _numOfIndices)
: numOfVertices{numOfVert},numOfIndices{_numOfIndices}{

    vertexArrayObject.genVAO();
    vertexBuffer.genVB(vertices,sizeof(vertices));
    vertexIndices.genVI(indexArray,sizeof(indexArray));
    //initModelMatrix();
}
funGT::Mesh::~Mesh(){
    //vertexArrayObject.release();
    //vertexBuffer.release();
    //vertexIndices.release(); 

}

void funGT::Mesh::initVAO(VERTEX *vertices, const unsigned int &numOfVert, GLuint *indexArray, const unsigned int& numOfIndices){

    this->numOfVertices = numOfVert; 
    this->numOfIndices = numOfIndices; 

    //vertexArrayObject.genVAO();
    //vertexBuffer.genVB(vertices,sizeof(this->numOfVertices));
    //vertexIndices.genVI(indexArray,sizeof(this->numOfIndices));

}  
void funGT::Mesh::initModelMatrix(){

    //INIT POS; ROT AND TRANS VALUES
    position = glm::vec3(0.f);
    rotation = glm::vec3(0.f);
    scale = glm::vec3(1.f);

    ModelMatrix = glm::mat4(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale); 
}
void funGT::Mesh::updateUniform(Shader *shader){

    shader->setUniformMat4fv("ModelMatrix",ModelMatrix);

} 
void funGT::Mesh::updateModelMatrix(){
    ModelMatrix = glm::mat4(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale); 
}
void funGT::Mesh::render(Shader *shader){
    //update uniforms;
    updateModelMatrix();
    updateUniform(shader);
    shader->Bind();
    //vertexArrayObject.build();
     //DRAW
    glDrawElements(GL_TRIANGLES,this->numOfIndices,GL_UNSIGNED_INT, 0);
}
