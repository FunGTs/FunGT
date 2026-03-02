#if !defined(_GEOMETRIES_H_)
#define _GEOMETRIES_H_
#include <vector>
#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
#include "Shaders/shader.hpp"

struct PrimitiveVertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

};


class Primitive {

private:
    std::vector<PrimitiveVertex> m_vertex;
    std::vector<GLuint> m_index;

    //Topology data for editing
    struct HalfEdge {
        uint32_t vertex;      // Which vertex this edge points TO
        uint32_t twin;        // Opposite half-edge (UINT32_MAX if boundary)
        uint32_t next;        // Next half-edge in the same face
        uint32_t face;        // Which face this edge belongs to
    };

    struct EditFace {
        uint32_t halfEdge;    // One (any) half-edge of this face
        bool selected;        // Is this face selected?
    };

    std::vector<HalfEdge> m_halfEdges;
    std::vector<EditFace> m_faces;
    std::vector<bool> m_vertexSelected;  // Per-vertex selection flags

    bool m_topologyDirty;  // Does topology need rebuilding?

public:
    VertexArrayObject m_vao;
    VertexBuffer m_vb;
    VertexIndex m_vi;
    Texture texture;


    public:
        Primitive();
        virtual ~Primitive();


        void set(const PrimitiveVertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices);
        void set(const PrimitiveVertex *vertices, const unsigned numOfvert);
        PrimitiveVertex *getVertices();
        GLuint* getIndices();
        unsigned getNumOfVertices();
        unsigned getNumOfIndices();
        long unsigned sizeOfVertices();
        long unsigned sizeOfIndices();
        void setAttribs();
        void unsetAttribs();
        const std::vector<PrimitiveVertex>& getVertices() const;
        const std::vector<unsigned int>& getIndices() const;
        // Geometry-specific virtuals
        virtual void setData() = 0;
        // NEW: Topology management
        void buildTopology();
        bool isTopologyBuilt() const { return !m_topologyDirty; }

        // Selection
        void clearSelection();
        void selectVertex(uint32_t idx, bool additive = false);
        void selectFace(uint32_t idx, bool additive = false);
        const std::vector<bool>& getVertexSelection() const { return m_vertexSelected; }
        const std::vector<EditFace>& getFaces() const { return m_faces; }
        //Drawing wireframe and vertices for edit mode: all primitives will use the same method, so we can implement it here in the base class
        void drawWireframe();
        void drawVertices();
        // GPU sync
        void updateGPUBuffers();

        // Geometry operations
        void recalculateNormals();
        // Graphics initialization
        void setTexture(const std::string &pathToTexture);
        void InitGraphics();
        virtual void InstancedDraw(Shader& shader, int instanceCount) {

        }
        // Pure virtual draw method
        virtual void draw() = 0;
}; 



#endif // _GEOMETRIES_H_

