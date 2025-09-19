#if !defined(MACRO)
#define MACRO
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stack> 
#include <memory>
#include <assimp/version.h>

#include "../Mesh/mesh.hpp"
#include "../Renderable/renderable.hpp"
#include "../DataPaths/datapaths.hpp"
class Model  {

    public:
        Model(); 
        Model(const std::string &path);
        virtual ~Model();
    
    //Methods
        void draw(Shader &shader);
        virtual void loadModel(const std::string &fullPath); 
        void Info();  
         // Setter declaration for m_dirPath
        void setDirPath(const std::string& dirPath);
        // Getter declaration for m_dirPath
        const std::string& getDirPath() const;
        void processAssimpScene(aiNode * node, const aiScene *scene);
        void createShader(std::string vertex_shader, std::string fragment_shader); 
        
   
        void draw();
        Shader &getShader();
        
    protected:
    //Members
        std::vector<std::unique_ptr<Mesh>> m_vMesh; 
        std::string m_dirPath; 
        std::vector<Texture> m_loadedTextures;
        Shader m_shader;
    
    private:
       
        glm::mat4 m_viewMatrix = glm::mat4(1.f); 
        glm::mat4 m_projectionMatrix  = glm::mat4(1.f); 


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
        
        
      
 
    
};



#endif // MACRO
