#if !defined(_SCENE_LIGHT_HPP_)
#define _SCENE_LIGHT_HPP_

#include "include/glmath.hpp"
#include <string>

enum class SceneLightType {
    Point,
    Sun,
    Spot,
    Area
};

struct SceneLight {
    SceneLightType   type = SceneLightType::Point;
    std::string name = "Light";
    glm::vec3   position = glm::vec3(0.f, 5.f, 0.f);
    glm::vec3   color = glm::vec3(1.f, 1.f, 1.f);
    float       power = 10.f;

    // Point
    float       radius = 0.1f;

    // Sun
    glm::vec3   direction = glm::vec3(0.f, -1.f, 0.f);

    // Spot
    float       innerAngle = 25.f;
    float       outerAngle = 45.f;

    // Area
    glm::vec3   normal = glm::vec3(0.f, -1.f, 0.f);
    glm::vec2   size = glm::vec2(1.f, 1.f);
};

#endif // _SCENE_LIGHT_HPP_