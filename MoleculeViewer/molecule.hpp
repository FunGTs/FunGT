#if !defined(_MOLECULE_HPP_)
#define _MOLECULE_HPP_

#include "Vector/vector3.hpp"
#include <vector>
#include <functional>
#include <string>

namespace fungt {

struct Atom {
    Vec3 position;
    float radius;
    std::string element;
};

inline Vec3 getCPKColor(const std::string& element) {
    if (element == "C")  return Vec3(0.5f, 0.5f, 0.5f);
    if (element == "N")  return Vec3(0.4f, 0.4f, 0.8f);
    if (element == "O")  return Vec3(0.8f, 0.2f, 0.2f);
    if (element == "H")  return Vec3(1.0f, 1.0f, 1.0f);
    if (element == "S")  return Vec3(0.8f, 0.8f, 0.2f);
    if (element == "P")  return Vec3(0.8f, 0.5f, 0.2f);
    if (element == "Cl") return Vec3(0.2f, 0.8f, 0.2f);
    return Vec3(0.5f, 0.5f, 0.5f);
}

struct Bond {
    Vec3 pointA;
    Vec3 pointB;
    float radius;
};

class Molecule {
    std::vector<Atom> m_atoms;
    std::vector<Bond> m_bonds;
    float m_blendK = 0.0f;

public:
    Molecule() = default;
    ~Molecule() = default;

    void addAtom(Vec3 position, float radius, const std::string& element = "");
    void addBond(Vec3 a, Vec3 b, float radius);
    void setBlend(float k);

    std::function<float(Vec3)> getField() const;
    void computeBounds(Vec3& min, Vec3& max, float padding = 0.5f) const;

    const std::vector<Atom>& getAtoms() const { return m_atoms; }
    const std::vector<Bond>& getBonds() const { return m_bonds; }
    void clear();
};

} // namespace fungt

#endif // _MOLECULE_HPP_
