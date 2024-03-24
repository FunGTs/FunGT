#if !defined(_PYRAMID_H_)
#define _PYRAMID_H_
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
//For texturized pyramids

class Pyramid{

private: 
        VertexArrayObject vao; 
        VertexBuffer vertexBuffer; 
        VertexIndex vertexIndex;
        Texture texture; 

public:
    Pyramid(); 
    Pyramid(const std::string  &path);
    ~Pyramid();


    void draw();
    void create(const std::string &path); 



};

#endif // _PYRAMID_H_
