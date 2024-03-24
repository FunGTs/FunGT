#include "square.hpp"

Square::Square(const std::string  &path)
: Primitive(){
    //Square with  texture
    Vertex vertices[] = 
    {
        //POSITION                         //COLOR                  //Texcoords        
        glm::vec3( -1.0f,1.0,0.0f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),
        glm::vec3(-1.0f, -1.0f, 0.0f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),
        glm::vec3(1.0f,  -1.0f, 0.0f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
        glm::vec3(1.0f,  1.0f, 0.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f)
    };
    unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
    GLuint indices[] = {

        0, 1, 2,
        0 , 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices,indices,nOfIndices);
    vao.genVAO();
    vertexBuffer.genVB(vertices,sizeof(vertices));
    vertexIndex.genVI(indices,sizeof(indices));



        //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
        glEnableVertexAttribArray(0);
        //COLOR
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        glEnableVertexAttribArray(1);
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glEnableVertexAttribArray(2);

    //Texture
    texture.genTexture(path);
    texture.active();
    texture.bind();

    //All binded above must be released
    vao.unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    vertexBuffer.release(); 


}
Square::Square()
: Primitive(){
    //Square with no texture
    Vertex vertices[] = 
    {
        //POSITION                         //COLOR                  //Texcoords        
        glm::vec3( -1.0f,1.0,0.0f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),
        glm::vec3(-1.0f, -1.0f, 0.0f),      glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),
        glm::vec3(1.0f,  -1.0f, 0.0f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f), 
        glm::vec3(1.0f,  1.0f, 0.0f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f)
    };
    unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
    GLuint indices[] = {

        0, 1, 2,
        0 , 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices,indices,nOfIndices);
    vao.genVAO();
    vertexBuffer.genVB(vertices,sizeof(vertices));
    vertexIndex.genVI(indices,sizeof(indices));



        //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
        glEnableVertexAttribArray(0);
        //COLOR
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        glEnableVertexAttribArray(1);
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glEnableVertexAttribArray(2);

    //All binded above must be released
    vao.unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    vertexBuffer.release(); 


}
void Square::draw(){
    //texture.bind();
    vao.bind();
    glDrawElements(GL_TRIANGLES,this->getNumOfIndices(), GL_UNSIGNED_INT,0);
}
Square::~Square(){

}