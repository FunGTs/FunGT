#include "random.hpp"

float randomFloat(int seed) {
    unsigned int state = seed * 1664525 + 1013904223;
    return static_cast<float>(state % 1000) / 1000.f;
}

float random(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
