#if !defined(MACRO)
#define MACRO
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stack> 
#include <memory>
#include <assimp/version.h>

#include "../Mesh/mesh.hpp"
class Model{

    public:
        Model(); 
        Model(const std::string &path);
        virtual ~Model();
    
    //Methods
        void draw(Shader &shader);
        virtual void loadModel(const std::string &fullPath); 
        void Info();  

    protected:
    //Members
        std::vector<std::unique_ptr<Mesh>> m_vMesh; 
        std::string m_dirPath; 
        std::vector<Texture> m_loadedTextures;

    //Methods

        void processNodes(aiNode * node, const aiScene *scene); 
        
        virtual std::unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene); 
        std::vector<Texture > loadTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
        std::vector<Material> loadMaterials(aiMaterial *mat);
    protected: 
        virtual std::vector<funGTVERTEX> getVertices(aiMesh *mesh, const aiScene *scene);
        std::vector<GLuint> getIndices(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture > getTextures(aiMesh *mesh, const aiScene *scene);
        std::vector<Material> getMaterials(aiMesh *mesh, const aiScene *scene);
        void processAssimpScene(aiNode * node, const aiScene *scene);
      
 
    
};



#endif // MACRO
