#include "manifold_collision.hpp"

// Static member definition
std::array<std::array<ManifoldFunc, ShapeTypeCount>, ShapeTypeCount> ManifoldCollision::m_dispatchTable = {};
