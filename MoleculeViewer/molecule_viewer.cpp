#include "molecule_viewer.hpp"
#include "molecule_gpu_field.hpp"
#include "MarchingCubes/gpu/mc_gpu.hpp"
#include <new>
#include "Path_Manager/path_manager.hpp"
#include <iostream>
#include <vector>
#include <limits>

MoleculeViewer::MoleculeViewer(fungt::MC_MODE mode) : Surface(mode) {}

MoleculeViewer::~MoleculeViewer() = default;

void MoleculeViewer::setBlend(float k) {
    m_molecule.setBlend(k);
}

void MoleculeViewer::rebuild() {
    if (isGPU) {
        createGPUInteropBuffer();
        return;
    }

    m_mc->execute();

    const auto& mcVerts = m_mc->getVertices();
    const auto& indices = m_mc->getIndices();
    const auto& atoms = m_molecule.getAtoms();

    std::vector<fungt::MCVertex> verts(mcVerts.size());
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

    createGPUBuffer(verts, indices);

    m_buffer->unbindVAO();
}

std::shared_ptr<MoleculeViewer> MoleculeViewer::create(const std::string& xyzPath, fungt::MC_MODE mode) {
    auto viewer = std::shared_ptr<MoleculeViewer>(new MoleculeViewer(mode));

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

    if (mode == fungt::MC_MODE::GPU) {
        viewer->m_mc->setGridSampler([molecule = &viewer->m_molecule](MarchingCubesGPU& mc) {
            auto& queue = mc.getQueue();
            std::vector<fungt::GPUAtom> hostAtoms;
            for (const auto& atom : molecule->getAtoms()) {
                auto color = fungt::getCPKColor(atom.element);
                hostAtoms.push_back({atom.position.x, atom.position.y, atom.position.z, atom.radius,
                                     color.x, color.y, color.z});
            }
            std::vector<fungt::GPUBond> hostBonds;
            for (const auto& bond : molecule->getBonds()) {
                hostBonds.push_back({bond.pointA.x, bond.pointA.y, bond.pointA.z,
                                     bond.pointB.x, bond.pointB.y, bond.pointB.z, bond.radius});
            }
            auto release = [queue](auto* ptr) {
                if (ptr) sycl::free(ptr, queue);
            };
            std::shared_ptr<fungt::GPUAtom> atoms;
            std::unique_ptr<fungt::GPUBond, decltype(release)> bonds(nullptr, release);
            if (!hostAtoms.empty()) {
                atoms.reset(sycl::malloc_device<fungt::GPUAtom>(hostAtoms.size(), queue), release);
                if (!atoms) throw std::bad_alloc();
                queue.memcpy(atoms.get(), hostAtoms.data(), hostAtoms.size() * sizeof(fungt::GPUAtom)).wait_and_throw();
            }
            if (!hostBonds.empty()) {
                bonds.reset(sycl::malloc_device<fungt::GPUBond>(hostBonds.size(), queue));
                if (!bonds) throw std::bad_alloc();
                queue.memcpy(bonds.get(), hostBonds.data(), hostBonds.size() * sizeof(fungt::GPUBond)).wait_and_throw();
            }
            auto field = fungt::makeMoleculeGPUField(atoms.get(), hostAtoms.size(),
                                                    bonds.get(), hostBonds.size(), molecule->getBlend());
            mc.createGridBuffer(field);

            const auto* deviceAtoms = atoms.get();
            const auto atomCount = hostAtoms.size();
            const float initialDistance = std::numeric_limits<float>::max();
            auto color = [=](float x, float y, float z) {
                float minDistance = initialDistance;
                glm::vec3 result(0.5f);
                for (std::size_t i = 0; i < atomCount; ++i) {
                    float dx = x - deviceAtoms[i].x;
                    float dy = y - deviceAtoms[i].y;
                    float dz = z - deviceAtoms[i].z;
                    float distance = sycl::sqrt(dx * dx + dy * dy + dz * dz);
                    if (distance < minDistance) {
                        minDistance = distance;
                        result = glm::vec3(deviceAtoms[i].r, deviceAtoms[i].g, deviceAtoms[i].b);
                    }
                }
                return result;
            };
            mc.setTriangleGenerator([atoms, color](MarchingCubesGPU& target, unsigned int vboId) {
                target.generateTriangles(vboId, color);
            });
        });
    } else {
        viewer->setField(viewer->m_molecule.getField());
    }

    fungt::Vec3 min, max;
    viewer->m_molecule.computeBounds(min, max, 0.5f);
    std::cout << "Bounds: (" << min.x << ", " << min.y << ", " << min.z << ") to ("
              << max.x << ", " << max.y << ", " << max.z << ")" << std::endl;
    viewer->sampleScalarFieldInside(min, max);

    return viewer;
}
