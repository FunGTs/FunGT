#if !defined(_IMGUI_SET_UP)
#define _IMGUI_SET_UP
#include "../include/prequisites.hpp"
#include "../include/glmath.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
void imguiSetup(GLFWwindow* window);
void imguiNewFrame();
void imguiRender();
void imguiCleanUp();
void imguiFrameBasic(glm::vec3 &position, glm::vec3 &rotation);


#endif // _IMGUI_SET_UP
