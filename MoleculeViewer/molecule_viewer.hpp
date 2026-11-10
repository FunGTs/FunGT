#if !defined(_MOLECULE_VIEWER_HPP_)
#define _MOLECULE_VIEWER_HPP_

#include "Surface/surface.hpp"
#include "molecule.hpp"
#include "mol_reader.hpp"
#include <memory>
#include <string>

class MoleculeViewer : public Surface {
    fungt::Molecule m_molecule;

    MoleculeViewer();

public:
    ~MoleculeViewer();

    void setBlend(float k);
    void rebuild() override;

    static std::shared_ptr<MoleculeViewer> create(const std::string& xyzPath);
};

#endif // _MOLECULE_VIEWER_HPP_
