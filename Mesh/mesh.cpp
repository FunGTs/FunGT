#include "mesh.hpp"

Mesh::Mesh(){
    std::cout<<"Mesh Default Destructor"<<std::endl; 
}

Mesh::Mesh(const std::vector<funGTVERTEX> &inVertex,const std::vector<GLuint> &inIndex,const std::vector<Texture> &inTexture)
: m_vertex(std::move(inVertex)),m_index{std::move(inIndex)}, m_texture{inTexture}{
     
     //Calls the init mesh to populate the VAO, VBO and EBO
     this->initMesh();
     
}
Mesh::Mesh(const std::vector<funGTVERTEX> &inVertex,const std::vector<GLuint> &inIndex,const std::vector<Material> &inMaterial)
: m_vertex(std::move(inVertex)),m_index{std::move(inIndex)}, m_material{inMaterial}{
      //Calls the init mesh to populate the VAO, VBO and EBO
     this->initMesh();
}
Mesh::~Mesh()
{
    // std::cout<<"Mesh Destructor"<<std::endl;
}
//Methods
void Mesh::initMesh() {
   //  //This method initialize a Mesh
    m_vao.genVAO(); //Generates a Vertex array object
    m_vb.genVB(); //Generates the Vertex Buffer
    m_vi.genVI(); //Generates the Vertex Buffer


    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(&m_vertex[0],m_vertex.size()*sizeof(Vertex));

    m_vi.bind(); 
    m_vi.indexData(&m_index[0],static_cast<unsigned int>(m_index.size()));
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index.size() * sizeof(unsigned int), &m_index[0], GL_STATIC_DRAW);

    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        //  offsetof(s,m) takes as its first argument a struct and as its second argument a 
        // variable name of the struct. 
        // The macro returns the byte offset of that variable from the start of the struct.
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
        glEnableVertexAttribArray(0);
        //COLOR
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        glEnableVertexAttribArray(1);
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glEnableVertexAttribArray(2);
    //All binded above must be released
      m_vao.unbind();
  
}
void Mesh::draw(Shader &shader){

   int numOfTextures = m_texture.size(); //How many textures do we have? 

    unsigned int diffuseL = 1; 
    unsigned int specularL = 1;
    //std::cout<<"This mesh contains : "<< numOfTextures<<std::endl; 
    for(unsigned int i=0; i<numOfTextures; i++){
      
        m_texture[i].active(i);
        //glActiveTexture(GL_TEXTURE0 + i);
        std::cout<<"Texture : "<<m_texture[i].getTypeName()<<" activated"<<std::endl; 
        std::string iter; //Asign a number at the end of the name
        if(m_texture[i].getTypeName()=="texture_diffuse"){
            iter = std::to_string(diffuseL++);
        }
        else if(m_texture[i].getTypeName()=="texture_specular"){
            iter = std::to_string(specularL++); 
        }
        std::string textName = m_texture[i].getTypeName()+iter; //Builds the full name of the texture
        std::cout<<"Sending : "<<textName<<" to the shader"<<std::endl;
        std::cout<<"Texture ID : "<<m_texture[i].getID()<<std::endl; 
        shader.set1i(i,textName);//Send texture to the shader
        //m_texture[i].active();
        m_texture[i].bind();
        //glBindTexture(GL_TEXTURE_2D, static_cast<unsigned int>(m_texture[i].getID())); 
    } 
    //loading the materials
    for(int i=0; i<m_material.size(); i++){
        m_material[i].sendToShader(shader);
    }

       //Draw your mesh!
        m_vao.bind();
        //glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_index.size()), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);
        m_vao.unbind();
        //glActiveTexture(GL_TEXTURE0); 
}
