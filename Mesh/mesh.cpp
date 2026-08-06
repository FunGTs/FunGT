#include "mesh.hpp"

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
    std::cout<<"Mesh Destructor"<<std::endl;
}
//Methods
void Mesh::initMesh() {
    m_buffer = GPUBuffer::create();

    m_buffer->genVAO();
    m_buffer->bindVAO();

    m_buffer->create(BufferType::Vertex, m_vertex.data(), m_vertex.size() * sizeof(funGTVERTEX));
    m_buffer->create(BufferType::Index,  m_index.data(),  m_index.size()  * sizeof(unsigned int));

    // layout baked into the VAO once
    m_buffer->applyFormat(funGTVERTEX::getFormat());

    m_buffer->unbindVAO();
}
void Mesh::InitOGLBuffers()
{
    initMesh();
}
void Mesh::draw(Shader &shader){

   int numOfTextures = m_texture.size(); //How many textures do we have? 

    unsigned int diffuseL = 1; 
    unsigned int specularL = 1;
    // ADD THIS LINE HERE:
    shader.setUniform1i("hasTexture", numOfTextures > 0 ? 1 : 0);
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

    m_buffer->bindVAO();
    m_buffer->drawIndexed((m_index.size() / 3) * 3);
    m_buffer->unbindVAO();

}

