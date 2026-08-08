#if !defined(_OPENGL_SHADER_HPP_)
#define _OPENGL_SHADER_HPP_

#include "../shader.hpp"
#include "../../include/prerequisites.hpp"

class OpenGLShader : public Shader {
public:
    OpenGLShader();
    ~OpenGLShader() override;

    void create(const std::string& pathVert, const std::string& pathFrag) override;
    void create(const std::string& pathVert, const std::string& pathFrag, const std::string& pathGeom) override;

    void Bind()   override;
    void unBind() override;

    void setUniform1i(const std::string& name, int value) override;
    void setUniform1f(float value, const std::string& name) override;
    void set1i(int value, const std::string& name) override;
    void setUniformVec1f(float value, const std::string& name) override;
    void setUniformVec2f(glm::fvec2 value, const std::string& name) override;
    void setUniformVec3f(glm::fvec3 value, const std::string& name) override;
    void setUniformVec4f(glm::fvec4 value, const std::string& name) override;
    void setVec4(glm::fvec4 value, const std::string& name) override;
    void setUniform4f(float r, float g, float b, const std::string& name) override;
    void setMat3fv(glm::mat3 value, const std::string& name, bool transpose = false) override;
    void setMat4fv(glm::mat4 value, const std::string& name, bool transpose = false) override;
    void setUniformMat4fv(const std::string& name, const glm::mat4& mat) override;

private:
    GLuint m_idP = 0;

    std::string loadShaderFromSource(const std::string& path);
    GLuint      loadShader(bool& error, GLenum type, const std::string& source);
    void        linkProgram(GLuint vShader, GLuint geomShader, GLuint fShader);
};

#endif // _OPENGL_SHADER_HPP_
