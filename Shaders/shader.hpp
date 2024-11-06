#if !defined(_SHADERS_H_)
#define _SHADERS_H_
#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"
#include <variant>
class Shader{

    GLuint idP; 

    public:
        Shader(); 
        Shader(std::string pathVert, std::string pathFrag, std::string pathgeom);
        Shader(std::string pathVert, std::string pathFrag);
        ~Shader();

      std::string loadShaderFromSource(std::string& source);
      GLuint loadShader(bool &error,GLenum type, std::string& source);
      void linkProgram(GLuint vShader, GLuint geomShader, GLuint fShader);
      void Bind();
      void unBind();
      void create(std::string pathVert, std::string pathFrag);
      void create(std::string pathVert, std::string pathFrag, std::string pathgeom); 
      //Setting the uniforms
      void setUniform1i(const std::string &name, int value);
      void setUniform1f(float value,const std::string &name);
      void set1i(GLint value, std::string name);
      void setUniformVec3f(glm::fvec3 value,std::string name);
      void setUniformVec2f(glm::fvec2 value,std::string name);
      void setUniformVec1f(GLfloat value,std::string name);
      void setUniformVec4f(glm::fvec4 value, std::string name);
      void setUniform4f(GLfloat r, GLfloat g, GLfloat b, std::string name);
      void setMat4fv(glm::mat4 value, std::string name,GLboolean transpose);
      void setUniformMat4fv(std::string name, const glm::mat4 &proj);
      void setVec4(glm::fvec4 value, std::string name);
      void setMat3fv(glm::mat3 value, std::string name, GLboolean transpose);



};

#endif // _SHADERS_H_
