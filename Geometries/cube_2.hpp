#if !defined(_CUBE_H_)
#define _CUBE_H_
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
//For texturized cubes


class Cube{

    private: 
        VertexArrayObject vao; 
        VertexBuffer vertexBuffer; 
        VertexIndex vertexIndex;
        Texture texture;  

    public:
        Cube();  
        Cube(const std::string  &path);
        ~Cube();
        void draw();
        void create(const std::string  &path); 

};

#endif // _CUBE_H_
