#if !defined(_SHADERS_H_)
#define _SHADERS_H_
#include "../include/glmath.hpp"
#include "../Renders/display_graphics.hpp"
#include <memory>
#include <string>

class Shader {
public:
    virtual ~Shader() = default;

    static std::unique_ptr<Shader> create();

    virtual void create(const std::string& pathVert, const std::string& pathFrag) = 0;
    virtual void create(const std::string& pathVert, const std::string& pathFrag, const std::string& pathGeom) = 0;

    virtual void Bind()   = 0;
    virtual void unBind() = 0;

    virtual void setUniform1i(const std::string& name, int value) = 0;
    virtual void setUniform1f(float value, const std::string& name) = 0;
    virtual void set1i(int value, const std::string& name) = 0;
    virtual void setUniformVec1f(float value, const std::string& name) = 0;
    virtual void setUniformVec2f(glm::fvec2 value, const std::string& name) = 0;
    virtual void setUniformVec3f(glm::fvec3 value, const std::string& name) = 0;
    virtual void setUniformVec4f(glm::fvec4 value, const std::string& name) = 0;
    virtual void setVec4(glm::fvec4 value, const std::string& name) = 0;
    virtual void setUniform4f(float r, float g, float b, const std::string& name) = 0;
    virtual void setMat3fv(glm::mat3 value, const std::string& name, bool transpose = false) = 0;
    virtual void setMat4fv(glm::mat4 value, const std::string& name, bool transpose = false) = 0;
    virtual void setUniformMat4fv(const std::string& name, const glm::mat4& mat) = 0;
};

#endif // _SHADERS_H_
