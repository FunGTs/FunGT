typedef struct fgt_rng {
    ulong state;
    ulong increment;
} fgt_rng;

inline uint fgt_rng_next_u32(__private fgt_rng* rng)
{
    const ulong old_state = rng->state;
    rng->state = old_state * 6364136223846793005UL + rng->increment;

    const uint xorshifted =
        (uint)(((old_state >> 18U) ^ old_state) >> 27U);
    const uint rotation = (uint)(old_state >> 59U);

    return (xorshifted >> rotation) |
           (xorshifted << ((-rotation) & 31U));
}

inline fgt_rng fgt_rng_create(ulong seed, ulong sequence)
{
    fgt_rng rng;
    rng.state = 0UL;
    rng.increment = (sequence << 1U) | 1UL;
    fgt_rng_next_u32(&rng);
    rng.state += seed;
    fgt_rng_next_u32(&rng);
    return rng;
}

inline float fgt_rng_next_float(__private fgt_rng* rng)
{
    return (float)(fgt_rng_next_u32(rng) >> 8U) *
           (1.0f / 16777216.0f);
}
