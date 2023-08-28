#if !defined(_GRAPHIC_ENGINE_H_)
#define _GRAPHIC_ENGINE_H_
#include<iostream>
#include<fstream>
#include<string>
#include "../Mesh/fungtMesh.hpp"
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Geometries/square.hpp"
#include "../Geometries/cube.hpp"
#include "../Window/window.hpp"
enum shader_enum{};
class GraphicEngine{

    public:
        //Members:
        Window *mainWindow; 
        //Shaders
        std::vector<Shader*> shaders; 
        //Textures
        std::vector<Texture*> textures; 
        //Constructors 
        GraphicEngine();
        ~GraphicEngine();

    private: 
        glm::mat4 ViewMat;
        glm::vec3 camPosition; 
        glm::vec3 worldUp;
        glm::vec4 ProjectionMAt; 
        glm::vec4 ModelMat; 
        float fov; 
        float nearPlane;
        float farPlane;
        void initMatrices();
        void initShaders();
        void initTextures();
        void update();
        void render(); 




};


#endif // _ENGINE_H_
