#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "vn3d.h"

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

struct vn_generator {
    unsigned int *seeds;
    unsigned int width, height, depth;
    unsigned int octaves;
};

struct vn_generator* vn_make_generator (unsigned int octaves, unsigned int width,
                                        unsigned int height, unsigned int depth)
{
    struct vn_generator *generator = malloc (sizeof (struct vn_generator));
    generator->seeds = malloc (sizeof (unsigned int) * octaves);
    generator->width = width;
    generator->height = height;
    generator->depth = depth;
    generator->octaves = octaves;

    int i;
    for (i=0; i<octaves; i++)
        generator->seeds[i] = rand();

    return generator;
}

void vn_destroy_generator (struct vn_generator *generator)
{
    free (generator->seeds);
    free (generator);
}

#if LINEAR_INTERPOLATE
static unsigned int interpolate (unsigned int v1, unsigned int v2, unsigned int x, unsigned int shift)
{
    long vl1 = v1;
    long vl2 = (long)v2 - (long)v1;

    long r = (vl2 * x) >> shift;

    return vl1 + r;
}
#else
static unsigned int interpolate (unsigned int v1, unsigned int v2, unsigned int x, unsigned int shift)
{
    long vl1 = v1;
    long vl2 = (long)v2 - (long)v1;

    long tmp1 = x * x * vl2;
    tmp1 >>= 2*shift;

    long tmp2 = (3 << shift) - 2*x;
    tmp2 *= tmp1;
    tmp2 >>= shift;

    long res = vl1 + tmp2;
    return res;
}
#endif

static unsigned int value_noise_one_pass_3d (const struct vn_generator *generator, unsigned int x,
                                             unsigned int y, unsigned int z, unsigned int pass)
{
    unsigned int v000, v001, v010, v011;
    unsigned int v100, v101, v110, v111;
    unsigned int v00, v01, v10, v11;
    unsigned int v0, v1, v;

    unsigned int diffx, diffy, diffz;

    unsigned int xshift = generator->width - pass;
    unsigned int yshift = generator->height - pass;
    unsigned int zshift = generator->depth - pass;

    unsigned int xmask = (1<<xshift) - 1;
    unsigned int ymask = (1<<yshift) - 1;
    unsigned int zmask = (1<<zshift) - 1;

    unsigned int xidx = x >> xshift;
    unsigned int yidx = y >> yshift;
    unsigned int zidx = z >> zshift;

    v000 = lolrand (xidx,   yidx,   zidx, generator->seeds[pass]);
    v001 = lolrand (xidx+1, yidx,   zidx, generator->seeds[pass]);
    v010 = lolrand (xidx,   yidx+1, zidx, generator->seeds[pass]);
    v011 = lolrand (xidx+1, yidx+1, zidx, generator->seeds[pass]);

    v100 = lolrand (xidx,   yidx,   zidx+1, generator->seeds[pass]);
    v101 = lolrand (xidx+1, yidx,   zidx+1, generator->seeds[pass]);
    v110 = lolrand (xidx,   yidx+1, zidx+1, generator->seeds[pass]);
    v111 = lolrand (xidx+1, yidx+1, zidx+1, generator->seeds[pass]);

    diffx = x & xmask;
    diffy = y & ymask;
    diffz = z & zmask;

    v00 = interpolate (v000, v001, diffx, xshift);
    v01 = interpolate (v010, v011, diffx, xshift);
    v10 = interpolate (v100, v101, diffx, xshift);
    v11 = interpolate (v110, v111, diffx, xshift);

    v0 = interpolate (v00, v01, diffy, yshift);
    v1 = interpolate (v10, v11, diffy, yshift);

    v = interpolate (v0, v1, diffz, zshift);

    return v;
}

unsigned int vn_noise_3d (const struct vn_generator *generator,
                          unsigned int x, unsigned int y, unsigned int z)
{
    int i;
    unsigned long res = 0;

    for (i=0; i<generator->octaves; i++)
        res += value_noise_one_pass_3d (generator, x, y, z, i);

    return res / generator->octaves;
}

static unsigned int value_noise_one_pass_2d (const struct vn_generator *generator,
                                             unsigned int x, unsigned int y,
                                             unsigned int pass)
{
    unsigned int v00, v01, v10, v11;
    unsigned int v0, v1, v;

    unsigned int diffx, diffy;

    unsigned int xshift = generator->width - pass;
    unsigned int yshift = generator->height - pass;

    unsigned int xmask = (1<<xshift) - 1;
    unsigned int ymask = (1<<yshift) - 1;

    unsigned int xidx = x >> xshift;
    unsigned int yidx = y >> yshift;

    v00 = lolrand (xidx,   yidx,   0, generator->seeds[pass]);
    v01 = lolrand (xidx+1, yidx,   0, generator->seeds[pass]);
    v10 = lolrand (xidx,   yidx+1, 0, generator->seeds[pass]);
    v11 = lolrand (xidx+1, yidx+1, 0, generator->seeds[pass]);

    diffx = x & xmask;
    diffy = y & ymask;

    v0 = interpolate (v00, v01, diffx, xshift);
    v1 = interpolate (v10, v11, diffx, xshift);

    v = interpolate (v0, v1, diffy, yshift);

    return v;
}

unsigned int vn_noise_2d (const struct vn_generator *generator, unsigned int x, unsigned int y)
{
    int i;
    unsigned long res = 0;

    for (i=0; i<generator->octaves; i++)
        res += value_noise_one_pass_2d (generator, x, y, i);

    return res / generator->octaves;
}
