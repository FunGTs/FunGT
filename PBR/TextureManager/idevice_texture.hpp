#if !defined(_IDEVICE_TEXTURE_HPP)
#define _IDEVICE_TEXTURE_HPP
//#include "texture_types.hpp"
#include "Vector/vector3.hpp"
#include <string>
#include <vector>

class IDeviceTexture {
 public:
    virtual int loadTexture(const std::string& path) = 0;
    virtual int getTextureCount() const = 0;
    virtual void cleanup() = 0;
    virtual ~IDeviceTexture() = default;


};

#endif // _IDEVICE_TEXTURE_HPP
