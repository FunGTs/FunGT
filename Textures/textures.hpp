#if !defined(_TEXTURES_H_)
#define _TEXTURES_H_
#include<vector>
#include "../include/prerequisites.hpp" 

class Texture{

    public: 
        std::string name;
        unsigned int m_id;
        std::string m_type;
     
 
    private:
        unsigned int txt_ID;
        std::string txt_Path;
        unsigned char* txt_localBuffer;
        int txt_width, txt_height,txt_BBP;
        unsigned int type; 
        GLint textureUnit; 
    public:
        
        Texture();
        Texture(unsigned int type_texture); 
        Texture(const std::string  &path );
        Texture(const std::string  &path, GLenum type);
        ~Texture();

        void genTexture(const std::string  &path );
        void genTextureCubeMap(const std::vector<std::string> &pathVector);
        void active(unsigned int slot = 0); 
        void bind();
        void unBind();
        void Delete();
        int getID() const;    

        inline int getWidth() const {return txt_width;}
        inline int getHeight() const {return txt_height;}
        inline void setPath(std::string path) { this->txt_Path = path; }
        inline std::string getPath()const { return this->txt_Path;}
        inline void setTypeName(std::string textureType){ this->m_type = textureType; }
        inline void setTypeTexture(unsigned int type_texture){this->type = type_texture;}
        inline std::string getTypeName()const { return this->m_type; }
        GLint getTextureUnit() const{
            return this->textureUnit;
        }


};

#endif // _TEXTURES_H_