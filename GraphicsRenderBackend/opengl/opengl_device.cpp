#include "opengl_device.hpp"
#include <iostream>

void OpenGLDevice::setWindowHints() {
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#endif
}

void OpenGLDevice::init(GLFWwindow* window, int width, int height, const float clearColor[4]) {
    glfwMakeContextCurrent(window);

#ifdef __APPLE__
    if (glfwInit() != GL_TRUE)
        std::cerr << "GLFW re-init error on Apple" << std::endl;
#else
    if (glewInit() != GLEW_OK)
        std::cerr << "GLEW init error" << std::endl;
#endif

    glViewport(0, 0, width, height);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    std::cout << "GL Version  : " << glGetString(GL_VERSION)  << std::endl;
    std::cout << "GL Vendor   : " << glGetString(GL_VENDOR)   << std::endl;
    std::cout << "GL Renderer : " << glGetString(GL_RENDERER) << std::endl;
}

void OpenGLDevice::shutdown() {
    // GL teardown handled by glfwTerminate() in ~GraphicsTool
}

void OpenGLDevice::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDevice::endFrame(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

static GLenum toGLDepthFunc(DepthFunc f) {
    switch (f) {
        case DepthFunc::Less:      return GL_LESS;
        case DepthFunc::LessEqual: return GL_LEQUAL;
        case DepthFunc::Equal:     return GL_EQUAL;
        case DepthFunc::Greater:   return GL_GREATER;
        case DepthFunc::Always:    return GL_ALWAYS;
    }
    return GL_LESS;
}

static GLenum toGLBlend(BlendFactor f) {
    switch (f) {
        case BlendFactor::SrcAlpha:         return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::One:              return GL_ONE;
        case BlendFactor::Zero:             return GL_ZERO;
    }
    return GL_ONE;
}

void OpenGLDevice::setDepthMask(bool write) {
    glDepthMask(write ? GL_TRUE : GL_FALSE);
}

void OpenGLDevice::setDepthFunc(DepthFunc func) {
    glDepthFunc(toGLDepthFunc(func));
}

void OpenGLDevice::setBlendFunc(BlendFactor src, BlendFactor dst) {
    glBlendFunc(toGLBlend(src), toGLBlend(dst));
}