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
    m_vao.bind();

    m_vb.genVB(); //Generates the Vertex Buffer
    m_vb.bind();
    m_vb.bufferData(&m_vertex[0],m_vertex.size()*sizeof(Vertex));

    m_vi.genVI(); //Generates the Vertex Buffer
    m_vi.bind(); 
    //m_vi.indexData(&m_index[0],sizeof(&m_index[0])*m_index.size()); //Changed to a possible bug : segmentation fault
    m_vi.indexData(m_index.data(), m_index.size() * sizeof(m_index[0]));
    
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index.size() * sizeof(unsigned int), &m_index[0], GL_STATIC_DRAW);

    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        //  offsetof(s,m) takes as its first argument a struct and as its second argument a 
        // variable name of the struct. 
        // The macro returns the byte offset of that variable from the start of the struct.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
       
        //Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        

        // ids
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, maxBoneInfluencePerVertex, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_BoneIDs));
        
		// weights
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, maxBoneInfluencePerVertex, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_Weights));
        
    m_vao.unbind();
  
}
void Mesh::InitOGLBuffers()
{
    //  //This method initialize a Mesh

    m_vao.genVAO(); //Generates a Vertex array object
    m_vao.bind();

    m_vb.genVB(); //Generates the Vertex Buffer
    m_vb.bind();
    m_vb.bufferData(&m_vertex[0], m_vertex.size() * sizeof(Vertex));

    m_vi.genVI(); //Generates the Vertex Buffer
    m_vi.bind();
    //m_vi.indexData(&m_index[0],sizeof(&m_index[0])*m_index.size()); //Changed to a possible bug : segmentation fault
    m_vi.indexData(m_index.data(), m_index.size() * sizeof(m_index[0]));

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index.size() * sizeof(unsigned int), &m_index[0], GL_STATIC_DRAW);

    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        //  offsetof(s,m) takes as its first argument a struct and as its second argument a 
        // variable name of the struct. 
        // The macro returns the byte offset of that variable from the start of the struct.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

    //Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

    //TEXTURE COORDS
    //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));


    // ids
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, maxBoneInfluencePerVertex, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, maxBoneInfluencePerVertex, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_Weights));

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
        //std::cout<<"Texture : "<<m_texture[i].getTypeName()<<" activated"<<std::endl; 
        std::string iter; //Asign a number at the end of the name
        if(m_texture[i].getTypeName()=="texture_diffuse"){
            iter = std::to_string(diffuseL++);
        }
        else if(m_texture[i].getTypeName()=="texture_specular"){
            iter = std::to_string(specularL++); 
        }
        std::string textName = m_texture[i].getTypeName()+iter; //Builds the full name of the texture
        //std::cout<<"Sending : "<<textName<<" to the shader"<<std::endl;
        //std::cout<<"Texture ID : "<<m_texture[i].getID()<<std::endl; 
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
        //std::cout<<"Drawing Mesh with: "<< m_index.size() << " indices "<<std::endl;
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_index.size() / 3) * 3, GL_UNSIGNED_INT, (void*)0); 
        //glBindVertexArray(0);
        m_vao.unbind();
        //glActiveTexture(GL_TEXTURE0);
        // OpenGL Error Checking

}

std::vector<Triangle> Mesh::ToTriangle() const
{
    {
        std::vector<Triangle> triangle_vec;
        triangle_vec.reserve(m_index.size() / 3);

        for (size_t i = 0; i < m_index.size(); i += 3) {
            const funGTVERTEX& v0 = m_vertex[m_index[i + 0]];
            const funGTVERTEX& v1 = m_vertex[m_index[i + 1]];
            const funGTVERTEX& v2 = m_vertex[m_index[i + 2]];

            Triangle tri;
            tri.v0 = fungt::toFungtVec3(v0.position);
            tri.v1 = fungt::toFungtVec3(v1.position);
            tri.v2 = fungt::toFungtVec3(v2.position);

            // Flat face normal (correct for path tracing)
            fungt::Vec3 e1 = tri.v1 - tri.v0;
            fungt::Vec3 e2 = tri.v2 - tri.v0;
            tri.normal = e1.cross(e2).normalize();

            // Include material if present
            if (!m_material.empty()) {
                tri.material = m_material[0]; // Most meshes only have one
            }

            // Optional: handle texture-only meshes later when you add albedo maps
            // if (!m_textures.empty()) tri.albedoMap = m_textures[0].id;

            triangle_vec.push_back(std::move(tri));
        }

        return triangle_vec;
}

