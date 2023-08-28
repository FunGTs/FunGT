#include "graphicEngine.hpp"

GraphicEngine::GraphicEngine()
:fov{45.0},nearPlane{0.1},farPlane{90.0}{

} 
GraphicEngine::~GraphicEngine(){


}
void GraphicEngine::initMatrices(){

}
void GraphicEngine::initShaders(){
    this->shaders.push_back(new Shader {"../resources/vertex_core.glsl","../resources/fragment_core.glsl"});
}
void GraphicEngine::initTextures(){

}
void GraphicEngine::render(){

}
void GraphicEngine::update(){
    
}