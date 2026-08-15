#include "opengl_texture.hpp"

static GLenum toGLTarget(TextureType t) {
    switch (t) {
        case TextureType::Texture2D: return GL_TEXTURE_2D;
        case TextureType::CubeMap:   return GL_TEXTURE_CUBE_MAP;
    }
    return GL_TEXTURE_2D;
}

OpenGLTexture::OpenGLTexture(TextureType type)
    : m_type(type), m_glTarget(toGLTarget(type)) {}

OpenGLTexture::~OpenGLTexture() {
    destroy();
}

void OpenGLTexture::upload(const unsigned char* pixels, int width, int height, TextureColorSpace colorSpace){
    glGenTextures(1, &m_id);
    glBindTexture(m_glTarget, m_id);

    glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    const GLenum internalFormat =
        colorSpace == TextureColorSpace::SRGB
        ? GL_SRGB8_ALPHA8
        : GL_RGBA8;


    glTexImage2D(m_glTarget, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(m_glTarget);
    glBindTexture(m_glTarget, 0);
}

void OpenGLTexture::uploadFloat(const float* pixels, int width, int height) {
    glGenTextures(1, &m_id);
    glBindTexture(m_glTarget, m_id);

    glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(m_glTarget, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    glBindTexture(m_glTarget, 0);
}

void OpenGLTexture::uploadCubeMap(const std::vector<unsigned char*>& faces, int width, int height) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < faces.size(); i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
                     width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLTexture::allocateEmptyCubemap(int faceSize, bool mipmaps) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     faceSize, faceSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                     mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLTexture::bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_glTarget, m_id);
}

void OpenGLTexture::unbind() {
    glBindTexture(m_glTarget, 0);
}

void OpenGLTexture::destroy() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

uint32_t OpenGLTexture::getID() const {
    return m_id;
}
