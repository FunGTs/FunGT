#pragma once
#include <vector>
#include "texture_manager.hpp"

#include <unordered_map>

struct CPUTexture_obj {
    std::vector<unsigned char> data;
    int width = 0;
    int height = 0;
    int channels = 0;
};


// Inherits from BOTH
class CPUTexture : public ITextureManager<CPUTexture_obj>,
    public IDeviceTexture {
private:
    std::vector<CPUTexture_obj> m_textures;
    std::unordered_map<std::string, int> m_pathToIndex;

public:
    CPUTexture() = default;
    ~CPUTexture() override { cleanup(); }

    // Implement ITextureManager<CPUTexture>
    int loadTexture(const std::string& path) override {

    }
    std::vector<CPUTexture_obj> getTextureObjects() override { return m_textures; }
    int getTextureCount() const override { return static_cast<int>(m_textures.size()); }
    void cleanup() override { m_textures.clear(); m_pathToIndex.clear(); }

    // CPU-specific
    CPUTexture_obj* getTextureArray() { return m_textures.data(); }
};