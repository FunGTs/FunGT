#include "mesh.hpp"

void (*Mesh::s_gpuCacheBuild)(Mesh&) = nullptr;
void (*Mesh::s_gpuCacheFree)(MeshGPU*) = nullptr;
void (*Mesh::s_gpuCacheDraw)(MeshGPU&, size_t) = nullptr;

Mesh::Mesh(){
    std::cout<<"Mesh Default Destructor"<<std::endl; 
}

Mesh::Mesh(const std::vector<funGTVERTEX> &inVertex, const std::vector<uint32_t> &inIndex, std::vector<Texture> inTexture)
: m_vertex(std::move(inVertex)), m_index{std::move(inIndex)}, m_texture{std::move(inTexture)} {}

Mesh::Mesh(const std::vector<funGTVERTEX> &inVertex, const std::vector<uint32_t> &inIndex, const std::vector<Material> &inMaterial)
: m_vertex(std::move(inVertex)), m_index{std::move(inIndex)}, m_material{inMaterial} {}

Mesh::Mesh(const std::vector<funGTVERTEX>& inVertex,
    const std::vector<uint32_t>& inIndex,
    std::vector<Texture> inTexture,
    const std::vector<Material>& inMaterial)
    : m_vertex(std::move(inVertex)),
    m_index{std::move(inIndex)},
    m_texture{std::move(inTexture)},
    m_material{inMaterial} {}
Mesh::~Mesh()
{
    if (s_gpuCacheFree && m_gpuCache) {
        s_gpuCacheFree(m_gpuCache);
    }
    std::cout<<"Mesh Destructor"<<std::endl;
}
//Methods
void Mesh::attachGPUCache(MeshGPU* cache)
{
    m_gpuCache = cache;
}
void Mesh::InitOGLBuffers()
{
    if (s_gpuCacheBuild && !m_gpuCache) {
        s_gpuCacheBuild(*this);
    }
}
void Mesh::draw(Shader &shader){

   int numOfTextures = m_texture.size(); //How many textures do we have?

    unsigned int diffuseL = 1;
    unsigned int specularL = 1;
  
    shader.setUniform1i("hasTexture", numOfTextures > 0 ? 1 : 0);
    shader.set1i(0, "texture_diffuse1"); // keep this sampler pinned to unit 0 even when unused
    //std::cout<<"This mesh contains : "<< numOfTextures<<std::endl; 
    for(unsigned int i=0; i<numOfTextures; i++){
        std::string iter;
        if(m_texture[i].getTypeName()=="texture_diffuse"){
            iter = std::to_string(diffuseL++);
        }
        else if(m_texture[i].getTypeName()=="texture_specular"){
            iter = std::to_string(specularL++);
        }
        std::string textName = m_texture[i].getTypeName() + iter;
        shader.set1i(i, textName);
        m_texture[i].active(i); // activates slot + binds
    } 
    //loading the materials
    for(int i=0; i<m_material.size(); i++){
        
        m_material[i].sendToShader(shader);

    }

    if (s_gpuCacheDraw && m_gpuCache) {
        s_gpuCacheDraw(*m_gpuCache, m_index.size());
    }

}

