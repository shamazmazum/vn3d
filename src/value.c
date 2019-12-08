#include <stdlib.h>
#include <math.h>
#include "value.h"
#include "private.h"

struct vn_value_generator {
    VN_GENERATOR_METHODS
    unsigned int *seeds;
    unsigned int octaves;
    unsigned int grid_pow;
};

static void destroy_generator (struct vn_generator *gen);
static unsigned int noise_3d (const struct vn_generator *gen,
                              unsigned int x, unsigned int y, unsigned int z);
static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y);
static unsigned int noise_1d (const struct vn_generator *gen, unsigned int x);

struct vn_generator* vn_value_generator (unsigned int octaves, unsigned int grid_pow)
{
    struct vn_value_generator *generator;
    unsigned int i;

    /* Sanity checks */
    grid_pow = (octaves > grid_pow)? octaves: grid_pow;

    vn_errcode = ALL_OK;
    generator = malloc (sizeof (struct vn_value_generator));
    generator->seeds = malloc (sizeof (unsigned int) * octaves);
    generator->grid_pow = grid_pow;
    generator->octaves = octaves;
    generator->destroy_generator = destroy_generator;
    generator->noise_1d = noise_1d;
    generator->noise_2d = noise_2d;
    generator->noise_3d = noise_3d;

    for (i=0; i<octaves; i++)
        generator->seeds[i] = rand();

    return (struct vn_generator*)generator;
}


static void destroy_generator (struct vn_generator *gen)
{
    struct vn_value_generator *generator = (struct vn_value_generator*)gen;
    free (generator->seeds);
    free (generator);
}

static unsigned int interpolate (unsigned int v1, unsigned int v2, unsigned int x)
{
    long vl1 = v1;
    long vl2 = (long)v2 - (long)v1;

    long r = (vl2*x) >> 8;

    return vl1 + r;
}

#if LINEAR_INTERPOLATE
static unsigned int intfn (unsigned int x, unsigned int shift)
{
    return (x << 8) >> shift;
}
#else
static unsigned int intfn (unsigned int x, unsigned int shift)
{
    unsigned long tmp = (x * x) << 8;
    tmp >>= 2*shift;

    unsigned long res = (3 << shift) - 2*x;
    res *= tmp;
    res >>= shift;

    return res;
}
#endif

/*----Poor man's RNG--*/
static unsigned int lolrand (unsigned int x, unsigned int y, unsigned int z, unsigned int seed)
{
    unsigned int r1, r2, r3, r;

    r1 = x * 0x1B873593;
    r2 = y * 0x19088711;
    r3 = z * 0xB2D05E13;

    r = seed + r1 + r2 + r3;
    r ^= r >> 5;
    r *= 0xCC9E2D51;

    return r;
}
/*--------------------*/

static unsigned int value_noise_one_pass_3d (const struct vn_value_generator *generator, unsigned int x,
                                             unsigned int y, unsigned int z, unsigned int pass)
{
    unsigned int v000, v001, v010, v011;
    unsigned int v100, v101, v110, v111;
    unsigned int v00, v01, v10, v11;
    unsigned int v0, v1, v;

    unsigned int diffx, diffy, diffz;
    unsigned int intx, inty, intz;

    unsigned int shift = generator->grid_pow - pass;
    unsigned int mask = (1<<shift) - 1;

    unsigned int xidx = x >> shift;
    unsigned int yidx = y >> shift;
    unsigned int zidx = z >> shift;

    unsigned int seed = generator->seeds[pass];

    /*
     * NB: Smart compilers like clang will partially apply lolrand function
     * (e.g. lolrandx = lolrand(x, _)) to reduce amount of calculations. Only
     * the last shift and multiplication by 0xCC9E2D51 will be calculated all
     * eight times.
     */
    v000 = lolrand (xidx,   yidx,   zidx, seed);
    v001 = lolrand (xidx+1, yidx,   zidx, seed);
    v010 = lolrand (xidx,   yidx+1, zidx, seed);
    v011 = lolrand (xidx+1, yidx+1, zidx, seed);

    v100 = lolrand (xidx,   yidx,   zidx+1, seed);
    v101 = lolrand (xidx+1, yidx,   zidx+1, seed);
    v110 = lolrand (xidx,   yidx+1, zidx+1, seed);
    v111 = lolrand (xidx+1, yidx+1, zidx+1, seed);

    diffx = x & mask;
    diffy = y & mask;
    diffz = z & mask;

    intx = intfn (diffx, shift);
    inty = intfn (diffy, shift);
    intz = intfn (diffz, shift);

    v00 = interpolate (v000, v001, intx);
    v01 = interpolate (v010, v011, intx);
    v10 = interpolate (v100, v101, intx);
    v11 = interpolate (v110, v111, intx);

    v0 = interpolate (v00, v01, inty);
    v1 = interpolate (v10, v11, inty);

    v = interpolate (v0, v1, intz);

    return v;
}

static unsigned int noise_3d (const struct vn_generator *gen,
                              unsigned int x, unsigned int y, unsigned int z)
{
    const struct vn_value_generator *generator = (struct vn_value_generator*)gen;

    unsigned long i, res = 0;
    int shift = generator->octaves - 1;

    for (i=0; i<generator->octaves; i++) {
        res += (long)(value_noise_one_pass_3d (generator, x, y, z, i)) << shift;
        shift--;
    }

    return res / ((1<<generator->octaves) - 1);
}

static unsigned int value_noise_one_pass_2d (const struct vn_value_generator *generator,
                                             unsigned int x, unsigned int y,
                                             unsigned int pass)
{
    unsigned int v00, v01, v10, v11;
    unsigned int v0, v1, v;

    unsigned int diffx, diffy;
    unsigned int intx, inty;

    unsigned int shift = generator->grid_pow - pass;
    unsigned int mask = (1<<shift) - 1;

    unsigned int xidx = x >> shift;
    unsigned int yidx = y >> shift;

    unsigned int seed = generator->seeds[pass];

    /*
     * Again, clang optimizes these calls to lolrand a lot. For example, the
     * last multiplication 0xB2D05E13 is eliminated.
     */

    v00 = lolrand (xidx,   yidx,   0, seed);
    v01 = lolrand (xidx+1, yidx,   0, seed);
    v10 = lolrand (xidx,   yidx+1, 0, seed);
    v11 = lolrand (xidx+1, yidx+1, 0, seed);

    diffx = x & mask;
    diffy = y & mask;

    intx = intfn (diffx, shift);
    inty = intfn (diffy, shift);

    v0 = interpolate (v00, v01, intx);
    v1 = interpolate (v10, v11, intx);

    v = interpolate (v0, v1, inty);

    return v;
}

static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y)
{
    const struct vn_value_generator *generator = (struct vn_value_generator*)gen;

    unsigned long i, res = 0;
    int shift = generator->octaves - 1;

    for (i=0; i<generator->octaves; i++) {
        res += (long)(value_noise_one_pass_2d (generator, x, y, i)) << shift;
        shift--;
    }

    return res / ((1<<generator->octaves) - 1);
}

static unsigned int value_noise_one_pass_1d (const struct vn_value_generator *generator,
                                             unsigned int x, unsigned int pass)
{
    unsigned int v0, v1, v;

    unsigned int diffx, intx;

    unsigned int shift = generator->grid_pow - pass;
    unsigned int mask = (1<<shift) - 1;
    unsigned int xidx = x >> shift;

    unsigned int seed = generator->seeds[pass];

    /*
     * Again, clang optimizes these calls to lolrand a lot. For example, the
     * last multiplication 0xB2D05E13 is eliminated.
     */

    v0 = lolrand (xidx,   0,   0, seed);
    v1 = lolrand (xidx+1, 0,   0, seed);

    diffx = x & mask;
    intx = intfn (diffx, shift);

    v = interpolate (v0, v1, intx);

    return v;
}

static unsigned int noise_1d (const struct vn_generator *gen, unsigned int x)
{
    const struct vn_value_generator *generator = (struct vn_value_generator*)gen;

    unsigned long i, res = 0;
    int shift = generator->octaves - 1;

    for (i=0; i<generator->octaves; i++) {
        res += (long)(value_noise_one_pass_1d (generator, x, i)) << shift;
        shift--;
    }

    return res / ((1<<generator->octaves) - 1);
}
