#if !defined(_TEXTURES_H_)
#define _TEXTURES_H_
#include "../include/prequisites.hpp" 

class Texture{

    public: 
        std::string name; 
    private:
        unsigned int txt_ID;
        std::string txt_Path;
        unsigned char* txt_localBuffer;
        int txt_width, txt_height,txt_BBP;
        unsigned int type; 
        GLint textureUnit; 
    public:
        Texture(); 
        Texture(const std::string  &path );
        Texture(const std::string  &path, GLenum type);
        ~Texture();

        void genTexture(const std::string  &path );
        void active(unsigned int slot = 0); 
        void bind();
        void unBind();
        int getID() const;    

        inline int getWidth() const {return txt_width;}
        inline int getHeight() const {return txt_height;}
        GLint getTextureUnit() const{
            return this->textureUnit;
        }


};


#endif // _TEXTURES_H_
