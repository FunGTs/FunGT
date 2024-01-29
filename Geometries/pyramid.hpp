#if !defined(_PYRAMID_H_)
#define _PYRAMID_H_
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
//For texturized pyramids

class Pyramid{

private: 
        VAO vao; 
        VB vertexBuffer; 
        VI vertexIndex;
        Texture texture; 

public:
    Pyramid(const std::string  &path);
    ~Pyramid();


    void draw();



};

#endif // _PYRAMID_H_
