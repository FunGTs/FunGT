#if !defined(_GPU_VEC3_H_)
#define _GPU_VEC3_H_



#include <vector>
#include "vector3.hpp"

// Structure of Arrays for Vec3 data
namespace fungt
{
    struct gpuVec3 {
        std::vector<float> x;
        std::vector<float> y;
        std::vector<float> z;

        // Get size
        size_t size() const {
            return x.size();
        }

        // Reserve capacity
        void reserve(size_t n) {
            x.reserve(n);
            y.reserve(n);
            z.reserve(n);
        }

        // Clear all data
        void clear() {
            x.clear();
            y.clear();
            z.clear();
        }

        // Push a Vec3
        void push_back(const fungt::Vec3& v) {
            x.push_back(v.x);
            y.push_back(v.y);
            z.push_back(v.z);
        }

        // Get a Vec3 at index
        fungt::Vec3 get(size_t i) const {
            return fungt::Vec3(x[i], y[i], z[i]);
        }

        // Set a Vec3 at index
        void set(size_t i, const fungt::Vec3& v) {
            x[i] = v.x;
            y[i] = v.y;
            z[i] = v.z;
        }

        // Resize all arrays
        void resize(size_t n) {
            x.resize(n);
            y.resize(n);
            z.resize(n);
        }

        // Fill with value
        void fill(const fungt::Vec3& value) {
            std::fill(x.begin(), x.end(), value.x);
            std::fill(y.begin(), y.end(), value.y);
            std::fill(z.begin(), z.end(), value.z);
        }
    };


} // namespace fungt


#endif // _GPU_VEC3_H_
