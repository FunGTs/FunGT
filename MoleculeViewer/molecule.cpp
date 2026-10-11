#include "molecule.hpp"
#include <cmath>
#include <limits>
#include <algorithm>

namespace fungt {

static float sphereSDF(Vec3 p, Vec3 center, float radius) {
    return (p - center).length() - radius;
}

static float capsuleSDF(Vec3 p, Vec3 a, Vec3 b, float radius) {
    Vec3 pa = p - a;
    Vec3 ba = b - a;
    float h = std::clamp(pa.dot(ba) / ba.dot(ba), 0.0f, 1.0f);
    return (pa - ba * h).length() - radius;
}

static float smin(float a, float b, float k) {
    if (k <= 0.0f)
        return std::min(a, b);
    float h = std::max(k - std::abs(a - b), 0.0f) / k;
    return std::min(a, b) - h * h * k * 0.25f;
}

void Molecule::addAtom(Vec3 position, float radius, const std::string& element) {
    m_atoms.push_back({position, radius, element});
}

void Molecule::addBond(Vec3 a, Vec3 b, float radius) {
    m_bonds.push_back({a, b, radius});
}

void Molecule::setBlend(float k) {
    m_blendK = k;
}

void Molecule::clear() {
    m_atoms.clear();
    m_bonds.clear();
}

std::function<float(Vec3)> Molecule::getField() const {
    auto atoms = m_atoms;
    auto bonds = m_bonds;
    float blendK = m_blendK;

    return [atoms, bonds, blendK](Vec3 p) -> float {
        float d = std::numeric_limits<float>::max();

        for (const auto& atom : atoms) {
            float sd = sphereSDF(p, atom.position, atom.radius);
            d = smin(d, sd, blendK);
        }

        for (const auto& bond : bonds) {
            float cd = capsuleSDF(p, bond.pointA, bond.pointB, bond.radius);
            d = smin(d, cd, blendK);
        }

        return d;
    };
}

void Molecule::computeBounds(Vec3& min, Vec3& max, float padding) const {
    if (m_atoms.empty()) {
        min = Vec3(-1.0f, -1.0f, -1.0f);
        max = Vec3(1.0f, 1.0f, 1.0f);
        return;
    }

    min = Vec3(std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max());
    max = Vec3(std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest());

    for (const auto& atom : m_atoms) {
        float r = atom.radius + padding;
        min.x = std::min(min.x, atom.position.x - r);
        min.y = std::min(min.y, atom.position.y - r);
        min.z = std::min(min.z, atom.position.z - r);
        max.x = std::max(max.x, atom.position.x + r);
        max.y = std::max(max.y, atom.position.y + r);
        max.z = std::max(max.z, atom.position.z + r);
    }

    for (const auto& bond : m_bonds) {
        float r = bond.radius + padding;
        min.x = std::min(min.x, std::min(bond.pointA.x, bond.pointB.x) - r);
        min.y = std::min(min.y, std::min(bond.pointA.y, bond.pointB.y) - r);
        min.z = std::min(min.z, std::min(bond.pointA.z, bond.pointB.z) - r);
        max.x = std::max(max.x, std::max(bond.pointA.x, bond.pointB.x) + r);
        max.y = std::max(max.y, std::max(bond.pointA.y, bond.pointB.y) + r);
        max.z = std::max(max.z, std::max(bond.pointA.z, bond.pointB.z) + r);
    }
}

} // namespace fungt
