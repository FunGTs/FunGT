#if !defined(_FGT_RNG_H_)
#define _FGT_RNG_H_
#include "../gpu/include/fgt_cpu_device.hpp"
namespace fungt{

    class RNG{

    public:
        uint64_t state;
        uint64_t inc;

        fgt_device RNG(uint64_t seed = 1u, uint64_t sequence = 1u) {
            state = 0u;
            inc = (sequence << 1u) | 1u;
            nextU32();     // warmup
            state += seed;
            nextU32();     // warmup
        }
        fgt_device ~RNG(){

        }
        fgt_device uint32_t nextU32() {
            uint64_t old = state;
            state = old * 6364136223846793005ULL + inc;
            uint32_t xorshift = ((old >> 18u) ^ old) >> 27u;
            uint32_t rot = old >> 59u;
            return (xorshift >> rot) | (xorshift << ((-rot) & 31));
        }

        fgt_device float nextFloat() {
            return (nextU32() >> 8) * (1.0f / 16777216.0f);
        }

        fgt_device float nextFloat01() {
            return nextU32() * (1.0f / 4294967296.0f);
        }

    };



}

#endif // _FGT_RNG_H_
