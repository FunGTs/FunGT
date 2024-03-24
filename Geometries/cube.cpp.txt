#include "cube.hpp"

Cube::Cube()
: Primitive(){
    printf("USING CUBE\n");
Vertex vertices[] = 
{
    //FRONT
    //POSITION                         //COLOR                  //Texcoords        
    glm::vec3( 0.5f,0.5f,0.5f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,0.f),
    glm::vec3(-0.5f, 0.5f, -0.5f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,1.f),
    glm::vec3(-0.5f,  0.5f, 0.5f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
    glm::vec3(0.5f,  -0.5f, -0.5f),      glm::vec3(1.f,1.f,1.f),   glm::vec2(1.f,1.f),

    //BACK
    glm::vec3(-0.5f,-0.5f,-0.5f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,0.f),
    glm::vec3(0.5f, 0.5f, -0.5f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(1.f,0.f),
    glm::vec3(0.5f,  -0.5f, 0.5f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(0.f,1.f), 
    glm::vec3(-0.5f,  -0.5f, 0.5f),      glm::vec3(1.f,1.f,1.f),    glm::vec2(1.f,1.f)

};
unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
GLuint indices[] = {
    // front
		0, 1, 2,
		1, 3, 4,
		// back
		5, 6, 3,
		7, 3, 6,
		// top
		2, 4, 7,
		0, 7, 6,
		// bottom
		0, 5, 1,
		1, 5, 3,
		// right
		5, 0, 6,
		7, 4, 3,
		// left
		2,1, 4,
		0, 2, 7 
};
unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices,indices,nOfIndices);
}