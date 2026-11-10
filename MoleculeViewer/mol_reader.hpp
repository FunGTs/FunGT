#if !defined(_MOL_READER_HPP_)
#define _MOL_READER_HPP_

#include "molecule.hpp"
#include "atom_data.hpp"
#include <string>
#include <vector>

namespace fungt {

struct ParsedAtom {
    std::string symbol;
    Vec3 position;
    AtomInfo info;
};

class MolReader {
    std::vector<ParsedAtom> m_parsedAtoms;
    std::string m_comment;
    float m_bondThreshold = 1.3f;

public:
    MolReader() = default;
    ~MolReader() = default;

    bool parse(const std::string& xyzPath);

    void setBondThreshold(float factor);

    Molecule buildMolecule(float radiusScale = 0.6f, float bondRadius = 0.05f) const;

    const std::vector<ParsedAtom>& getParsedAtoms() const { return m_parsedAtoms; }
    const std::string& getComment() const { return m_comment; }
};

} // namespace fungt

#endif // _MOL_READER_HPP_
