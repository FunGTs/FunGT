#if !defined(_CUBE_H_)
#define _CUBE_H_
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
//For texturized cubes


class Cube{

    private: 
        VAO vao; 
        VB vertexBuffer; 
        VI vertexIndex;
        Texture texture;  

    public: 
        Cube(const std::string  &path);
        ~Cube();
        void draw(); 

};

#endif // _CUBE_H_
