#if !defined(_SHADERS_H_)
#define _SHADERS_H_
#include "../include/prequisites.hpp"
#include "../include/glmath.hpp"

class Shader{

    GLuint idP; 

    public:
        Shader(std::string pathVert, std::string pathFrag, std::string pathgeom);
        Shader(std::string pathVert, std::string pathFrag);
        ~Shader();

      std::string loadShaderFromSource(std::string& source);
      GLuint loadShader(GLenum type, std::string& source);
      void linkProgram(GLuint vShader, GLuint geomShader, GLuint fShader);
      void Bind();
      void unBind();
      //Setting the uniforms
      void setUniform1i(const std::string &name, int value);
      void set1i(GLint value, std::string name);
      void setUniformVec3f(glm::fvec3 value,std::string name);
      void setUniformVec2f(glm::fvec2 value,std::string name);
      void setUniformVec1f(GLfloat value,std::string name);
      void setMat4fv(glm::mat4 value, std::string name,GLboolean transpose);
      void setUniformMat4fv(std::string name, const glm::mat4 &proj);
      void setVec4(glm::fvec4 value, std::string name);
      void setMat3fv(glm::mat3 value, std::string name, GLboolean transpose);



};

#endif // _SHADERS_H_
