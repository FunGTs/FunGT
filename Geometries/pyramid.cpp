#include "pyramid.hpp"

Pyramid::Pyramid()
{
}

Pyramid::Pyramid(const std::string &path)
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    };

    vao.genVAO();
    vertexBuffer.genVB(vertices,sizeof(vertices));
    

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    texture.genTexture(path);
    texture.bind();

    //All binded above must be released
    vao.unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    vertexBuffer.release();
}
Pyramid::~Pyramid(){

}
void Pyramid::draw(){
    texture.active();
    texture.bind();
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

void Pyramid::create(const std::string &path)
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    };

    vao.genVAO();
    vertexBuffer.genVB(vertices,sizeof(vertices));
    

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    texture.genTexture(path);
    texture.bind();

    //All binded above must be released
    vao.unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    vertexBuffer.release();
}
