#if !defined(_MOLECULE_VIEWER_HPP_)
#define _MOLECULE_VIEWER_HPP_

#include "Surface/surface.hpp"
#include "molecule.hpp"
#include "mol_reader.hpp"
#include <memory>
#include <string>

class MoleculeViewer : public Surface {
    fungt::Molecule m_molecule;

    MoleculeViewer(fungt::MC_MODE mode);

public:
    ~MoleculeViewer();

    void setBlend(float k);
    void rebuild() override;

    static std::shared_ptr<MoleculeViewer> create(const std::string& xyzPath,
        fungt::MC_MODE mode = fungt::MC_MODE::CPU);
};

#endif // _MOLECULE_VIEWER_HPP_
