#include "mesh.hpp"
Mesh::Mesh(){
    std::cout<<"Mesh Default Destructor"<<std::endl; 
}

Mesh::Mesh(std::vector<funGTVERTEX> inVertex,std::vector<GLuint> inIndex,std::vector<Texture> inTexture){
     //Populate the mesh with the input vertices
    std::cout<<"Mesh Constructor"<<std::endl; 
     this->m_vertex = inVertex; 
     this->m_index = inIndex; 
     this->m_texture = inTexture; 

}
Mesh::~Mesh(){
    std::cout<<"Mesh Destructor"<<std::endl; 
}
//Methods
void Mesh::initMesh() {
     //This method initialize a Mesh
    m_vao.genVAO(); //Generates a Vertex array object
    m_vb.genVB(&m_vertex[0],m_vertex.size()*sizeof(funGTVERTEX));
    m_vi.genVI(&m_index[0],m_index.size()*sizeof(GLuint));

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
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    m_vb.release(); 


}
void Mesh::draw(Shader &shader){

    int numOfTextures = m_texture.size(); //How many textures do we have? 

    unsigned int diffuseL = 1; 
    unsigned int specularL = 1;

    for(unsigned int i=0; i<numOfTextures; i++){
      
        m_texture[i].active(i);

        std::string iter; //Asign a number at the end of the name
        if(m_texture[i].name=="diffuse"){
            iter = std::to_string(diffuseL++);
        }
        else if(m_texture[i].name=="specular"){
            iter = std::to_string(specularL++); 
        }
        std::string textName = "material."+m_texture[i].name+iter; //Builds the full name of the texture
        shader.set1i(i,textName);//Send texture to the shader
        m_texture[i].bind(); 
    } 

    //Draw your mesh!

    m_vao.bind(); //Bind the vertex array object
    glDrawElements(GL_TRIANGLES, m_index.size(), GL_UNSIGNED_INT, 0); 

    //return to the defaults: 

    


    

}