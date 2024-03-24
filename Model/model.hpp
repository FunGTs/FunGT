#if !defined(MACRO)
#define MACRO
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Mesh/mesh.hpp"
class Model{

    public:
        Model(); 
        Model(const std::string &path);
        ~Model();
    
    //Methods
        void draw();
        void loadModel(const std::string &fullPath); 
        void Info();  

    private:
    //Members
        std::vector<Mesh> m_vMesh; 
        std::string m_dirPath; 

    //Methods

        
        std::vector<funGTVERTEX> getVertices(aiMesh *mesh, const aiScene *scene);
        std::vector<GLuint> getIndices(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> getTextures(aiMesh *mesh, const aiScene *scene);
        void processNodes(aiNode * node, const aiScene *scene); 
        Mesh processMesh(aiMesh *mesh, const aiScene *scene); 
        std::vector<Texture> loadMaterials(aiMaterial *mat, aiTextureType type, std::string typeName); 
        
        





};



#endif // MACRO
