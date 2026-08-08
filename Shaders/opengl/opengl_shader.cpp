#include "opengl_shader.hpp"
#include <fstream>
#include <iostream>

OpenGLShader::OpenGLShader()
{
    std::cout << "OpenGLShader default constructor" << std::endl;
    m_idP = 0;
}

OpenGLShader::~OpenGLShader()
{
    std::cout << "OpenGLShader destructor" << std::endl;
    if (m_idP)
        glDeleteProgram(m_idP);
}

std::string OpenGLShader::loadShaderFromSource(const std::string& path)
{
    std::string temp, src;
    std::ifstream f(path);
    if (f.is_open()) {
        while (std::getline(f, temp))
            src += temp + "\n";
    } else {
        std::cout << "Error: " << path << " could not be opened\n";
    }
    return src;
}

GLuint OpenGLShader::loadShader(bool& error, GLenum type, const std::string& source)
{
    char infoLog[512];
    GLint success;
    GLuint shader = glCreateShader(type);
    std::string src = loadShaderFromSource(source);
    const GLchar* srcPtr = src.c_str();
    glShaderSource(shader, 1, &srcPtr, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILE_FAILED\n" << infoLog << std::endl;
        error = true;
    }
    return shader;
}

void OpenGLShader::linkProgram(GLuint vShader, GLuint geomShader, GLuint fShader)
{
    char infoLog[512];
    GLint success;
    std::cout << "LINKING" << std::endl;
    m_idP = glCreateProgram();
    glAttachShader(m_idP, vShader);
    if (geomShader)
        glAttachShader(m_idP, geomShader);
    glAttachShader(m_idP, fShader);
    glLinkProgram(m_idP);
    glGetProgramiv(m_idP, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_idP, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glValidateProgram(m_idP);
    glGetProgramiv(m_idP, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_idP, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VALIDATE_FAILED\n" << infoLog << std::endl;
        exit(0);
    }
    glUseProgram(0);
    std::cout << "leaving linking" << std::endl;
}

void OpenGLShader::create(const std::string& pathVert, const std::string& pathFrag)
{
    bool errorVS = false, errorFS = false;
    GLuint vShader = 0, geomShader = 0, fShader = 0;
    vShader = loadShader(errorVS, GL_VERTEX_SHADER, pathVert);
    if (errorVS) { std::cout << "Vertex Shader Error" << std::endl; exit(0); }
    fShader = loadShader(errorFS, GL_FRAGMENT_SHADER, pathFrag);
    if (errorFS) { std::cout << "Fragment Shader Error" << std::endl; exit(0); }
    linkProgram(vShader, geomShader, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    std::cout << "SUCCESS: Compilation of Shaders" << std::endl;
}

void OpenGLShader::create(const std::string& pathVert, const std::string& pathFrag, const std::string& pathGeom)
{
    bool error = false;
    GLuint vShader = 0, geomShader = 0, fShader = 0;
    vShader    = loadShader(error, GL_VERTEX_SHADER,   pathVert);
    geomShader = loadShader(error, GL_GEOMETRY_SHADER, pathGeom);
    fShader    = loadShader(error, GL_FRAGMENT_SHADER, pathFrag);
    linkProgram(vShader, geomShader, fShader);
    glDeleteShader(vShader);
    glDeleteShader(geomShader);
    glDeleteShader(fShader);
}

void OpenGLShader::Bind()   { glUseProgram(m_idP); }
void OpenGLShader::unBind() { glUseProgram(0); }

void OpenGLShader::setUniform1i(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(m_idP, name.c_str()), value);
}
void OpenGLShader::setUniform1f(float value, const std::string& name) {
    glUniform1f(glGetUniformLocation(m_idP, name.c_str()), value);
}
void OpenGLShader::set1i(int value, const std::string& name) {
    glUniform1i(glGetUniformLocation(m_idP, name.c_str()), value);
}
void OpenGLShader::setUniformVec1f(float value, const std::string& name) {
    glUniform1f(glGetUniformLocation(m_idP, name.c_str()), value);
}
void OpenGLShader::setUniformVec2f(glm::fvec2 value, const std::string& name) {
    glUniform2fv(glGetUniformLocation(m_idP, name.c_str()), 1, glm::value_ptr(value));
}
void OpenGLShader::setUniformVec3f(glm::fvec3 value, const std::string& name) {
    glUniform3fv(glGetUniformLocation(m_idP, name.c_str()), 1, glm::value_ptr(value));
}
void OpenGLShader::setUniformVec4f(glm::fvec4 value, const std::string& name) {
    glUniform4fv(glGetUniformLocation(m_idP, name.c_str()), 1, glm::value_ptr(value));
}
void OpenGLShader::setVec4(glm::fvec4 value, const std::string& name) {
    glUniform4fv(glGetUniformLocation(m_idP, name.c_str()), 1, glm::value_ptr(value));
}
void OpenGLShader::setUniform4f(float r, float g, float b, const std::string& name) {
    glUniform4f(glGetUniformLocation(m_idP, name.c_str()), r, g, b, 1.0f);
}
void OpenGLShader::setMat3fv(glm::mat3 value, const std::string& name, bool transpose) {
    glUniformMatrix3fv(glGetUniformLocation(m_idP, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}
void OpenGLShader::setMat4fv(glm::mat4 value, const std::string& name, bool transpose) {
    glUniformMatrix4fv(glGetUniformLocation(m_idP, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}
void OpenGLShader::setUniformMat4fv(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(m_idP, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
