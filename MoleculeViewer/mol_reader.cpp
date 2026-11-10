#include "mol_reader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace fungt {

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

bool MolReader::parse(const std::string& xyzPath) {
    m_parsedAtoms.clear();
    m_comment.clear();

    std::ifstream file(xyzPath);
    if (!file.is_open())
        return false;

    std::string line;

    if (!std::getline(file, line))
        return false;
    int atomCount = std::stoi(trim(line));

    if (!std::getline(file, line))
        return false;
    m_comment = trim(line);

    for (int i = 0; i < atomCount; ++i) {
        if (!std::getline(file, line))
            return false;

        std::istringstream iss(line);
        std::string symbol;
        float x, y, z;

        if (!(iss >> symbol >> x >> y >> z))
            return false;

        ParsedAtom atom;
        atom.symbol = symbol;
        atom.position = Vec3(x, y, z);
        atom.info = lookupAtom(symbol);
        m_parsedAtoms.push_back(atom);
    }

    return true;
}

void MolReader::setBondThreshold(float factor) {
    m_bondThreshold = factor;
}

Molecule MolReader::buildMolecule(float radiusScale, float bondRadius) const {
    Molecule mol;

    for (const auto& atom : m_parsedAtoms) {
        mol.addAtom(atom.position, atom.info.covalentRadius * radiusScale, atom.symbol);
    }

    for (size_t i = 0; i < m_parsedAtoms.size(); ++i) {
        for (size_t j = i + 1; j < m_parsedAtoms.size(); ++j) {
            const auto& a = m_parsedAtoms[i];
            const auto& b = m_parsedAtoms[j];

            float dist = (a.position - b.position).length();
            float sumRadii = a.info.covalentRadius + b.info.covalentRadius;

            if (dist < sumRadii * m_bondThreshold) {
                mol.addBond(a.position, b.position, bondRadius);
            }
        }
    }

    return mol;
}

} // namespace fungt
