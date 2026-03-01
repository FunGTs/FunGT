#include "primitives.hpp"

Primitive::Primitive(){

}

Primitive::~Primitive()
{
}
void Primitive::set(const PrimitiveVertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices){

    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
    for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->m_index.push_back(indices[i]);
    }


}
void Primitive::set(const PrimitiveVertex *vertices, const unsigned numOfvert)
{
    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
}
PrimitiveVertex *Primitive::getVertices()
{
    return this->m_vertex.data();
}
GLuint* Primitive::getIndices(){
    return this->m_index.data();
}
 unsigned Primitive::getNumOfVertices(){
    return this->m_vertex.size();
}
 unsigned Primitive::getNumOfIndices(){
    return this->m_index.size();
}
long unsigned Primitive::sizeOfVertices(){
    return sizeof(PrimitiveVertex)*this->m_vertex.size();
}
long unsigned Primitive::sizeOfIndices(){
    return sizeof(PrimitiveVertex)*this->m_index.size();
}

void Primitive::setAttribs()
{
    //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,position));
       
        //COLOR
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,normal));
        
        //TEXTURE COORDS
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,texcoord));
        
}

void Primitive::unsetAttribs()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

const std::vector<PrimitiveVertex>& Primitive::getVertices() const
{
    // TODO: insert return statement here
    return m_vertex;
}

const std::vector<unsigned int>& Primitive::getIndices() const
{
    // TODO: insert return statement here
    return m_index;
}

// Graphics initialization
void Primitive::setTexture(const std::string &pathToTexture)
{
    texture.genTexture(pathToTexture);
    texture.active();
    texture.bind();
}

void Primitive::InitGraphics()
{
    m_vao.genVAO();
    m_vb.genVB();
    m_vi.genVI();

    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(this->getVertices(), this->sizeOfVertices());

    this->setAttribs();

    // Upload index data if indices exist
    if (this->getNumOfIndices() > 0) {
        m_vi.bind();
        m_vi.indexData(this->getIndices(), this->sizeOfIndices());
    }

    m_vao.unbind();
    this->unsetAttribs();
    m_vb.unbind();
    if (this->getNumOfIndices() > 0) {
        m_vi.unbind();
    }
    buildTopology();
}
void Primitive::clearSelection()
{
    // Clear all vertex selection flags
    std::fill(m_vertexSelected.begin(), m_vertexSelected.end(), false);

    // Clear all face selection flags
    for (auto& face : m_faces) {
        face.selected = false;
    }
}

void Primitive::selectVertex(uint32_t idx, bool additive)
{
    if (idx >= m_vertex.size()) {
        std::cerr << "selectVertex: index " << idx << " out of range" << std::endl;
        return;
    }

    if (!additive) {
        clearSelection();
    }

    m_vertexSelected[idx] = true;
}

void Primitive::selectFace(uint32_t idx, bool additive)
{
    if (idx >= m_faces.size()) {
        std::cerr << "selectFace: index " << idx << " out of range" << std::endl;
        return;
    }

    if (!additive) {
        clearSelection();
    }

    m_faces[idx].selected = true;
}
void Primitive::updateGPUBuffers()
{
    if (m_vertex.empty()) {
        return;
    }

    // Bind VBO and re-upload vertex data
    m_vb.bind();
    m_vb.bufferData(this->getVertices(), this->sizeOfVertices());
    m_vb.unbind();

    // Bind EBO and re-upload index data (if we have indices)
    if (!m_index.empty()) {
        m_vi.bind();
        m_vi.indexData(this->getIndices(), this->sizeOfIndices());
        m_vi.unbind();
    }
}
void Primitive::recalculateNormals()
{
    if (m_vertex.empty() || m_index.empty()) {
        return;
    }

    // Zero out all vertex normals
    for (auto& v : m_vertex) {
        v.normal = glm::vec3(0.0f);
    }

    // Accumulate face normals into vertices (area-weighted)
    for (size_t i = 0; i < m_index.size(); i += 3) {
        uint32_t i0 = m_index[i];
        uint32_t i1 = m_index[i + 1];
        uint32_t i2 = m_index[i + 2];

        glm::vec3 v0 = m_vertex[i0].position;
        glm::vec3 v1 = m_vertex[i1].position;
        glm::vec3 v2 = m_vertex[i2].position;

        // Face normal (cross product gives area-weighted normal)
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        // Accumulate into vertex normals (don't normalize yet)
        m_vertex[i0].normal += faceNormal;
        m_vertex[i1].normal += faceNormal;
        m_vertex[i2].normal += faceNormal;
    }

    // Normalize all vertex normals
    for (auto& v : m_vertex) {
        if (glm::length(v.normal) > 0.0001f) {
            v.normal = glm::normalize(v.normal);
        }
    }
}
void Primitive::buildTopology()
{
    // Clear existing topology
    m_halfEdges.clear();
    m_faces.clear();
    m_vertexSelected.clear();

    if (m_index.empty() || m_vertex.empty()) {
        m_topologyDirty = false;
        return;
    }

    // Initialize vertex selection flags
    m_vertexSelected.resize(m_vertex.size(), false);

    // Count triangles
    size_t numTriangles = m_index.size() / 3;
    m_faces.resize(numTriangles);

    // Allocate space for half-edges (3 per triangle)
    m_halfEdges.resize(numTriangles * 3);

    // Build half-edges from triangles
    for (size_t faceIdx = 0; faceIdx < numTriangles; ++faceIdx) {
        uint32_t i0 = m_index[faceIdx * 3 + 0];
        uint32_t i1 = m_index[faceIdx * 3 + 1];
        uint32_t i2 = m_index[faceIdx * 3 + 2];

        // Three half-edges for this triangle
        uint32_t he0 = faceIdx * 3 + 0;
        uint32_t he1 = faceIdx * 3 + 1;
        uint32_t he2 = faceIdx * 3 + 2;

        // Half-edge 0: i0 -> i1
        m_halfEdges[he0].vertex = i1;
        m_halfEdges[he0].next = he1;
        m_halfEdges[he0].face = faceIdx;
        m_halfEdges[he0].twin = UINT32_MAX;  // Will find twins next

        // Half-edge 1: i1 -> i2
        m_halfEdges[he1].vertex = i2;
        m_halfEdges[he1].next = he2;
        m_halfEdges[he1].face = faceIdx;
        m_halfEdges[he1].twin = UINT32_MAX;

        // Half-edge 2: i2 -> i0
        m_halfEdges[he2].vertex = i0;
        m_halfEdges[he2].next = he0;
        m_halfEdges[he2].face = faceIdx;
        m_halfEdges[he2].twin = UINT32_MAX;

        // Store one half-edge reference in the face
        m_faces[faceIdx].halfEdge = he0;
        m_faces[faceIdx].selected = false;
    }

    // Find twin half-edges
    // For each half-edge, search for its opposite
    for (size_t i = 0; i < m_halfEdges.size(); ++i) {
        if (m_halfEdges[i].twin != UINT32_MAX) {
            continue;  // Already has a twin
        }

        // This half-edge goes: vertexStart -> vertexEnd
        // Find the triangle index and position within triangle for edge i
        uint32_t faceIdx = i / 3;
        uint32_t edgeInFace = i % 3;

        // Get the starting vertex of this half-edge
        uint32_t vertexStart;
        if (edgeInFace == 0) {
            vertexStart = m_index[faceIdx * 3 + 0];
        }
        else if (edgeInFace == 1) {
            vertexStart = m_index[faceIdx * 3 + 1];
        }
        else {
            vertexStart = m_index[faceIdx * 3 + 2];
        }

        uint32_t vertexEnd = m_halfEdges[i].vertex;

        // Search for twin: an edge going vertexEnd -> vertexStart
        for (size_t j = i + 1; j < m_halfEdges.size(); ++j) {
            uint32_t otherFaceIdx = j / 3;
            uint32_t otherEdgeInFace = j % 3;

            uint32_t otherVertexStart;
            if (otherEdgeInFace == 0) {
                otherVertexStart = m_index[otherFaceIdx * 3 + 0];
            }
            else if (otherEdgeInFace == 1) {
                otherVertexStart = m_index[otherFaceIdx * 3 + 1];
            }
            else {
                otherVertexStart = m_index[otherFaceIdx * 3 + 2];
            }

            uint32_t otherVertexEnd = m_halfEdges[j].vertex;

            // Check if this is the twin
            if (vertexStart == otherVertexEnd && vertexEnd == otherVertexStart) {
                m_halfEdges[i].twin = j;
                m_halfEdges[j].twin = i;
                break;
            }
        }
    }

    m_topologyDirty = false;

    std::cout << "Topology built: " << m_vertex.size() << " vertices, "
        << m_faces.size() << " faces, "
        << m_halfEdges.size() << " half-edges" << std::endl;
}