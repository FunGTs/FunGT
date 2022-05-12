#include "textures.hpp"
#include "../vendor/stb_image/stb_image.h"

Texture::Texture(const std::string & path)
: type{GL_TEXTURE_2D},txt_Path{path}, txt_localBuffer{nullptr}, txt_width{0}, txt_height{0}, txt_BBP{0}{
    
    /**
     * OpenGL expects textures pixels start at the bootom left 
    */
    stbi_set_flip_vertically_on_load(1);
    txt_localBuffer = stbi_load(txt_Path.c_str(), &txt_width, &txt_height, &txt_BBP,4);

    glGenTextures(1, &txt_ID);
    glBindTexture(type,txt_ID);

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    


    glTexImage2D(type, 0, GL_RGBA8, txt_width, txt_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt_localBuffer );
    glBindTexture(type,0);

    if(txt_localBuffer){
        std::cout << "\nOk to load: " << std::endl;
        printf("%s\n",txt_Path.c_str());

        stbi_image_free(txt_localBuffer);
    }
    else
    {

        std::cout << "\nError: Failed to load texture" << std::endl;

        std::cout << stbi_failure_reason() << std::endl;

    }

}
Texture::Texture(const std::string & path, GLenum _type)
: type{_type},txt_Path{path}, txt_localBuffer{nullptr}, txt_width{0}, txt_height{0}, txt_BBP{0}{
    
    /**
     * OpenGL expects textures pixels start at the bootom left 
    */
    stbi_set_flip_vertically_on_load(1);
    txt_localBuffer = stbi_load(txt_Path.c_str(), &txt_width, &txt_height, &txt_BBP,4);

    glGenTextures(1, &txt_ID);
    glBindTexture(type,txt_ID);

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    


    glTexImage2D(type, 0, GL_RGBA8, txt_width, txt_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt_localBuffer );
    glBindTexture(type,0);

    if(txt_localBuffer){
        std::cout << "\nOk to load: " << std::endl;
        printf("%s\n",txt_Path.c_str());

        stbi_image_free(txt_localBuffer);
    }
    else
    {

        std::cout << "\nError: Failed to load texture" << std::endl;

        std::cout << stbi_failure_reason() << std::endl;

    }

}
Texture::~Texture(){
    glDeleteTextures(1, &txt_ID);
}
void Texture::bind(unsigned int slot /* = 0*/){
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(type,txt_ID);
}
void Texture::unBind(){
    glBindTexture(type,0);
}
int Texture::getID() const{
    return this->txt_ID;    
}

