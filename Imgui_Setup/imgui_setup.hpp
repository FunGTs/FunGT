#if !defined(_IMGUI_SET_UP)
#define _IMGUI_SET_UP
#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"
#include "../include/imgui_headers.hpp"
void imguiSetup(GLFWwindow* window);
void imguiNewFrame();
void imguiRender();
void imguiCleanUp();
void imguiFrameBasic(glm::vec3 &position, glm::vec3 &rotation);


#endif // _IMGUI_SET_UP
