#include "molecule_viewer.hpp"
#include "Path_Manager/path_manager.hpp"
#include <iostream>
#include <vector>
#include <limits>

MoleculeViewer::MoleculeViewer() = default;

MoleculeViewer::~MoleculeViewer() = default;

void MoleculeViewer::setBlend(float k) {
    m_molecule.setBlend(k);
}

void MoleculeViewer::rebuild() {
    m_mc.execute();

    const auto& mcVerts = m_mc.getVertices();
    const auto& indices = m_mc.getIndices();
    const auto& atoms = m_molecule.getAtoms();

    std::vector<SurfaceVertex> verts(mcVerts.size());
    for (size_t i = 0; i < mcVerts.size(); ++i) {
        verts[i].position = mcVerts[i].position;
        verts[i].normal = mcVerts[i].normal;

        fungt::Vec3 pos(mcVerts[i].position.x, mcVerts[i].position.y, mcVerts[i].position.z);
        float minDist = std::numeric_limits<float>::max();
        std::string nearestElement = "";

        for (const auto& atom : atoms) {
            float dist = (pos - atom.position).length();
            if (dist < minDist) {
                minDist = dist;
                nearestElement = atom.element;
            }
        }

        fungt::Vec3 cpk = fungt::getCPKColor(nearestElement);
        verts[i].color = glm::vec3(cpk.x, cpk.y, cpk.z);
    }

    m_indexCount = static_cast<unsigned int>(indices.size());

    m_buffer = GPUBuffer::create();
    m_buffer->genVAO();
    m_buffer->bindVAO();
    m_buffer->create(BufferType::Vertex, verts.data(),
        verts.size() * sizeof(SurfaceVertex));
    m_buffer->create(BufferType::Index, indices.data(),
        indices.size() * sizeof(unsigned int));

    m_buffer->applyFormat(SurfaceVertex::getFormat());

    m_buffer->unbindVAO();
}

std::shared_ptr<MoleculeViewer> MoleculeViewer::create(const std::string& xyzPath) {
    auto viewer = std::shared_ptr<MoleculeViewer>(new MoleculeViewer());

    std::string fullPath = getAssetPath(xyzPath);
    std::cout << "Loading molecule: " << fullPath << std::endl;

    fungt::MolReader reader;
    if (reader.parse(fullPath)) {
        viewer->m_molecule = reader.buildMolecule();
        std::cout << "Loaded " << viewer->m_molecule.getAtoms().size() << " atoms, "
                  << viewer->m_molecule.getBonds().size() << " bonds" << std::endl;
    } else {
        std::cerr << "ERROR: Failed to parse XYZ file: " << fullPath << std::endl;
    }

    viewer->setField(viewer->m_molecule.getField());

    fungt::Vec3 min, max;
    viewer->m_molecule.computeBounds(min, max, 0.5f);
    std::cout << "Bounds: (" << min.x << ", " << min.y << ", " << min.z << ") to ("
              << max.x << ", " << max.y << ", " << max.z << ")" << std::endl;
    viewer->sampleScalarFieldInside(min, max);

    return viewer;
}
