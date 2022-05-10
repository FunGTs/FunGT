#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
void imguiSetup(GLFWwindow* window);
void imguiNewFrame();
void imguiRender();
void imguiCleanUp();
void imguiFrameBasic(glm::vec3 &position, glm::vec3 &rotation);