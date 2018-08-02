#include <stdlib.h>
#include <math.h>
#include "value.h"
#include "private.h"

struct vn_value_generator {
    VN_GENERATOR_METHODS
    unsigned int *seeds;
    unsigned int width, height, depth;
    unsigned int octaves;
};

static void destroy_generator (struct vn_generator *gen);
static unsigned int noise_3d (const struct vn_generator *gen,
                              unsigned int x, unsigned int y, unsigned int z);
static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y);

struct vn_generator* vn_value_generator (unsigned int octaves, unsigned int width,
                                         unsigned int height, unsigned int depth)
{
    struct vn_value_generator *generator;

    /* Sanity checks */
    if (octaves > width ||
        octaves > height ||
        octaves > depth) {
        vn_errcode = TOO_MANY_OCTAVES;
        generator = NULL;
    } else {
        vn_errcode = ALL_OK;
        generator = malloc (sizeof (struct vn_value_generator));
        generator->seeds = malloc (sizeof (unsigned int) * octaves);
        generator->width = width;
        generator->height = height;
        generator->depth = depth;
        generator->octaves = octaves;
        generator->destroy_generator = destroy_generator;
        generator->noise_2d = noise_2d;
        generator->noise_3d = noise_3d;

        int i;
        for (i=0; i<octaves; i++)
            generator->seeds[i] = rand();
    }

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

    r1 = x ^ (x >> 1);
    r2 = y ^ (y >> 1);
    r3 = z ^ (z >> 1);

    r1 *= 0x1B873593;
    r2 *= 0x19088711;
    r3 *= 0xB2D05E13;

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

    unsigned int xshift = generator->width - pass;
    unsigned int yshift = generator->height - pass;
    unsigned int zshift = generator->depth - pass;

    unsigned int xmask = (1<<xshift) - 1;
    unsigned int ymask = (1<<yshift) - 1;
    unsigned int zmask = (1<<zshift) - 1;

    unsigned int xidx = x >> xshift;
    unsigned int yidx = y >> yshift;
    unsigned int zidx = z >> zshift;

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

    diffx = x & xmask;
    diffy = y & ymask;
    diffz = z & zmask;

    intx = intfn (diffx, xshift);
    inty = intfn (diffy, yshift);
    intz = intfn (diffz, zshift);

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

    int i;
    unsigned long res = 0;

    for (i=0; i<generator->octaves; i++)
        res += value_noise_one_pass_3d (generator, x, y, z, i);

    return res / generator->octaves;
}

static unsigned int value_noise_one_pass_2d (const struct vn_value_generator *generator,
                                             unsigned int x, unsigned int y,
                                             unsigned int pass)
{
    unsigned int v00, v01, v10, v11;
    unsigned int v0, v1, v;

    unsigned int diffx, diffy;
    unsigned int intx, inty;

    unsigned int xshift = generator->width - pass;
    unsigned int yshift = generator->height - pass;

    unsigned int xmask = (1<<xshift) - 1;
    unsigned int ymask = (1<<yshift) - 1;

    unsigned int xidx = x >> xshift;
    unsigned int yidx = y >> yshift;

    unsigned int seed = generator->seeds[pass];

    /*
     * Again, clang optimizes these calls to lolrand a lot. For example, the
     * last multiplication 0xB2D05E13 is eliminated.
     */

    v00 = lolrand (xidx,   yidx,   0, seed);
    v01 = lolrand (xidx+1, yidx,   0, seed);
    v10 = lolrand (xidx,   yidx+1, 0, seed);
    v11 = lolrand (xidx+1, yidx+1, 0, seed);

    diffx = x & xmask;
    diffy = y & ymask;

    intx = intfn (diffx, xshift);
    inty = intfn (diffy, yshift);

    v0 = interpolate (v00, v01, intx);
    v1 = interpolate (v10, v11, intx);

    v = interpolate (v0, v1, inty);

    return v;
}

static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y)
{
    const struct vn_value_generator *generator = (struct vn_value_generator*)gen;

    int i;
    unsigned long res = 0;

    for (i=0; i<generator->octaves; i++)
        res += value_noise_one_pass_2d (generator, x, y, i);

    return res / generator->octaves;
}
