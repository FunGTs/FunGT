#if !defined(_TEXTURE_MANAGER_HPP)
#define _TEXTURE_MANAGER_HPP
//#include "texture_types.hpp"
#include <string>
#include <vector>



template<typename TextureObjectType>
class ITextureManager{
public:
    virtual int loadTexture(const std::string &path) = 0; 
    virtual std::vector<TextureObjectType> getTextureObjects() = 0;
    virtual int getTextureCount() const = 0;
    virtual void cleanup() = 0; 
    virtual ~ITextureManager() = default;
};

class IDeviceTexture {
 public:
    virtual int loadTexture(const std::string& path) = 0;
    virtual int getTextureCount() const = 0;
    virtual void cleanup() = 0;
    virtual ~IDeviceTexture() = default;


};

#endif // _TEXTURE_MANAGER_HPP
