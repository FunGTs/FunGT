#include "graphicsTool.hpp"


GraphicsTool::GraphicsTool(int _width, int _height)
: m_width{_width}, m_height{_height},m_Windowname{"FunGT"}{
    std::cout<<"GraphicsTool constructor "<<std::endl; 
}

GraphicsTool::~GraphicsTool(){
    std::cout<<"GraphicsTool destructor"<<std::endl; 
    // Delete window before ending the program
    glfwDestroyWindow(m_Window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}
int GraphicsTool::initGL(){
    std::cout << "Init backend" << std::endl;

    if (!glfwInit())
        return -1;

    m_renderDevice = GraphicsRenderDevice::create(DisplayGraphics::GetBackend());
    GraphicsRenderDevice::Register(m_renderDevice.get());
    m_renderDevice->setWindowHints();

    m_Window = glfwCreateWindow(m_width, m_height, m_Windowname.c_str(), NULL, NULL);
    if (!m_Window) {
        glfwTerminate();
        return -1;
    }

    setWindowUserPointer(this);
    glfwGetFramebufferSize(m_Window, &m_frameBufferWidth, &m_frameBufferHeight);
    glfwSwapInterval(0);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
        GraphicsTool* instance = static_cast<GraphicsTool*>(glfwGetWindowUserPointer(window));
        if (instance) instance->onMouseMove(xpos, ypos);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
        GraphicsTool* instance = static_cast<GraphicsTool*>(glfwGetWindowUserPointer(window));
        if (instance) instance->onMouseScroll(xoffset, yoffset);
    });

    m_renderDevice->init(m_Window, m_frameBufferWidth, m_frameBufferHeight, m_colors);

    return 1;
}


void GraphicsTool::setWindowUserPointer(void* pointer) {
        glfwSetWindowUserPointer(m_Window, pointer);
}

void GraphicsTool::render(const std::function<void()> &renderLambda)
{
    while (!glfwWindowShouldClose(m_Window)) {
        m_renderDevice->beginFrame();

        this->update(renderLambda);
        this->renderGUI();

        m_renderDevice->endFrame(m_Window);
        glfwPollEvents();
    }
}


void GraphicsTool::update(const std::function<void()> &renderLambda)
{
    renderLambda();
}

void GraphicsTool::renderGUI()
{
    
}

