#include "perlin_noise.hpp"

    double PerlinNoise::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
double PerlinNoise::lerp(double t, double a, double b) {
    return a + t * (b - a);
}
double PerlinNoise::noise2d(double x, double y) {
        // Find unit square containing the point
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        // Relative coordinates within the unit square
        x -= std::floor(x);
        y -= std::floor(y);

        // Compute fade curves
        double u = fade(x);
        double v = fade(y);

        // Hash coordinates of the square's corners
        int aa = perm[X     + perm[Y    ]];
        int ab = perm[X     + perm[Y + 1]];
        int ba = perm[X + 1 + perm[Y    ]];
        int bb = perm[X + 1 + perm[Y + 1]];

        // Add blended results from the corners
        double result = lerp(v, 
                             lerp(u, grad(aa, x, y), grad(ba, x - 1, y)),
                             lerp(u, grad(ab, x, y - 1), grad(bb, x - 1, y - 1)));

        return (result + 1.0) / 2.0; // Normalize to [0, 1]
}
double PerlinNoise::noise3d(double x, double y, double z, int octaves, double persistence)
{ 
    /*

        An octave is one layer of Perlin Noise
        Multiple octaves create more detailed noise
        Higher frequency = finer details
        Lower amplitude = smoother blending
    
     */
    if(octaves > 0 ){
        double total = 0;
        double maxValue = 0;  // Used for normalization
        double amplitude = 1.0;
        double frequency = 1.0;

        for (int i = 0; i < octaves; i++) {
            total += PerlinNoise::pnoise3d(x * frequency, y * frequency, z*frequency) * amplitude;
            maxValue += amplitude;

            amplitude *= persistence;  // Decrease amplitude
            frequency *= 2.0;          // Increase frequency
        }

        return (total / maxValue + 1.0) / 2.0;  // Normalize to [0,1]
    }
    else{
        return PerlinNoise::pnoise3d(x, y, z);
    }
}
double PerlinNoise::pnoise3d(double x, double y, double z)
{
    // Find the unit cube that contains the point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    // Find relative x, y, z of the point in the cube
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Compute fade curves for each of x, y, z
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    // Add blended results from the 8 corners of the cube
    double lerp_u1 = lerp(u, grad(perm[AA], x, y, z), grad(perm[BA], x - 1, y, z));
    double lerp_u2 = lerp(u, grad(perm[AB], x, y - 1, z), grad(perm[BB], x - 1, y - 1, z));
    double lerp_u3 = lerp(u, grad(perm[AA + 1], x, y, z - 1), grad(perm[BA + 1], x - 1, y, z - 1));
    double lerp_u4 = lerp(u, grad(perm[AB + 1], x, y - 1, z - 1), grad(perm[BB + 1], x - 1, y - 1, z - 1));

    double lerp_v1 = lerp(v, lerp_u1, lerp_u2);
    double lerp_v2 = lerp(v, lerp_u3, lerp_u4);
    double result = lerp(w, lerp_v1, lerp_v2);
    return (result + 1.0) / 2.0; // Normalize to [0, 1]
}
double PerlinNoise::grad(int hash, double x, double y, double z)
{
     int h = hash & 15;       // Take the last 4 bits of the hash
        double u = h < 8 ? x : y; // Use x or y based on the hash
        double v = h < 4 ? y : (h == 12 || h == 14 ? x : z); // Choose y or z
        return ((h & 1) == 0 ? u : -u) +
               ((h & 2) == 0 ? v : -v);
}
double PerlinNoise::grad(int hash, double x, double y) {
        // Gradient function for 2D
        int h = hash & 3; // Use only the first 2 bits
        double u = h < 2 ? x : y; // Select x or y based on h
        double v = h < 2 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
// Static permutation table (doubled for wrapping)
const int PerlinNoise::perm[512] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
    140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177,
    33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165,
    71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211,
    133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63,
    161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135,
    130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
    250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152,
    2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
    19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235,
    249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176,
    115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29,
    24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, // Repeat for wrapping
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
    140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177,
    33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165,
    71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211,
    133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63,
    161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135,
    130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
    250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152,
    2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
    19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235,
    249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176,
    115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29,
    24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};