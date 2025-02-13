#if !defined(_PERLIN_NOISE_H_)
#define _PERLIN_NOISE_H_
#include <cmath>
#include <vector>
#include <iostream>
class PerlinNoise {

    static const int perm[512];
    static double fade(double t);

    // Linear interpolation
    static double lerp(double t, double a, double b);
    
    static double grad(int hash, double x, double y, double z);
    static double grad(int hash, double x, double y);
    static double pnoise3d(double x, double y, double z);


    public:
       
        static double noise2d(double x, double y);
        static double noise3d(double x, double y, double z, int octaves = 0.0, double persistence = 0.0); 

};
// Static permutation table (doubled for wrapping)



#endif // _PERLIN_NOISE_H_
