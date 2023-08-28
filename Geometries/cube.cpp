#include "cube.hpp"

Cube::Cube()
: Primitive(){
    printf("USING CUBE\n");
Vertex vertices[] = 
{
    //FRONT
    //POSITION                         //COLOR                  //Texcoords        
    glm::vec3( -0.5f,-0.5f,-0.5f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,0.f),
    glm::vec3(-0.5f, 0.5f, -0.5f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(1.f,0.f),
    glm::vec3(0.5f,  0.5f, -0.5f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
    glm::vec3(0.5f,  -0.5f, -0.5f),      glm::vec3(1.f,1.f,1.f),   glm::vec2(1.f,1.f),

    //BACK
    glm::vec3( 0.5f,-0.5f,0.5f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),
    glm::vec3(0.5f, 0.5f, 0.5f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),
    glm::vec3(-0.5f,  0.5f, 0.5f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
    glm::vec3(-0.5f,  -0.5f, 0.5f),      glm::vec3(1.f,1.f,1.f),    glm::vec2(1.f,1.f)

};
unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
GLuint indices[] = {
    // front
		0, 1, 2,
		2, 3, 0,
		// back
		4, 5, 6,
		6, 7, 4,
		// top
		1, 6, 5,
		5, 2, 1,
		// bottom
		7, 0, 3,
		3, 4, 7,
		// right
		3, 2, 5,
		5, 4, 3,
		// left
		7,6, 1,
		1, 0, 7 
};
unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices,indices,nOfIndices);
}