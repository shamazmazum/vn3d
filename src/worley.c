#include <stdlib.h>
#include <limits.h>
#include "worley.h"
#include "private.h"

struct vn_worley_generator {
    VN_GENERATOR_METHODS
    unsigned int ndots;
    unsigned int seed;
    unsigned int width, height, depth;
};

static void destroy_generator (struct vn_generator *gen);
static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y);
static unsigned int noise_3d (const struct vn_generator *gen, unsigned int x, unsigned int y, unsigned int z);

struct vn_generator* vn_worley_generator (unsigned int ndots, unsigned int width,
                                          unsigned int height, unsigned int depth)
{
    struct vn_worley_generator *generator = malloc (sizeof (struct vn_worley_generator));
    generator->width = width;
    generator->height = height;
    generator->depth = depth;
    generator->ndots = ndots;
    generator->seed = rand();
    generator->destroy_generator = destroy_generator;
    generator->noise_2d = noise_2d;
    generator->noise_3d = noise_3d;
    vn_errcode = ALL_OK;

    return (struct vn_generator*)generator;
}

static void destroy_generator (struct vn_generator *gen)
{
    free (gen);
}

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

static unsigned int xorshift32 (unsigned int state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    unsigned int x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}
/*--------------------*/

static unsigned int check_square (const struct vn_worley_generator *generator,
                           int sx, int sy,
                           int dx, int dy)
{
    unsigned int rnd = lolrand (sx, sy, 0, generator->seed);
    unsigned int ndots = generator->ndots;
    unsigned int i;

    unsigned int closest_dist = UINT_MAX;
    for (i=0; i<ndots; i++) {
        // Get 8-bit fixed point dot coordinates
        rnd = xorshift32 (rnd);
        unsigned int dotx = rnd & 0xff;
        unsigned int doty = (rnd >> 8) & 0xff;
        int x = dotx - dx;
        int y = doty - dy;
        unsigned int dist = x*x + y*y;
        closest_dist = (dist < closest_dist)? dist: closest_dist;
    }

    return closest_dist;
}

#define maybe_check_square(dist, xidx, yidx, x, y) do {                 \
        unsigned int dist2;                                             \
        if (dist < closest_dist) {                                      \
            dist2 = check_square (generator, xidx, yidx, x, y);         \
            closest_dist = (dist2 < closest_dist)? dist2: closest_dist; \
        }                                                               \
    } while (0)

static unsigned int noise_2d (const struct vn_generator *gen, unsigned int x, unsigned int y)
{
    struct vn_worley_generator *generator = (struct vn_worley_generator*) gen;

    unsigned int width = generator->width;
    unsigned int height = generator->height;

    unsigned int xidx = x >> width;
    unsigned int yidx = y >> height;

    // Translate x,y local coordinates to 8-bit fixed point
    unsigned int xmask = (1 << width) - 1;
    unsigned int ymask = (1 << height) - 1;

    unsigned int xdiff = x & xmask;
    xdiff <<= 8;
    xdiff >>= width;

    unsigned int ydiff = y & ymask;
    ydiff <<= 8;
    ydiff >>= height;

    unsigned int closest_dist = check_square (generator, xidx, yidx, xdiff, ydiff);

    int xa, xs, ya, ys;
    xa = xdiff - (1<<8);
    ya = ydiff - (1<<8);
    xs = xdiff + (1<<8);
    ys = ydiff + (1<<8);

    unsigned int xsq, ysq, xopsq, yopsq;
    xsq = xdiff*xdiff;
    ysq = ydiff*ydiff;
    xopsq = xa*xa;
    yopsq = ya*ya;

    // Check adjacent squares
    maybe_check_square (xsq,   xidx - 1, yidx, xs, ydiff);
    maybe_check_square (xopsq, xidx + 1, yidx, xa, ydiff);
    maybe_check_square (ysq,   xidx, yidx - 1, xdiff, ys);
    maybe_check_square (yopsq, xidx, yidx + 1, xdiff, ya);

    maybe_check_square (xsq + ysq,     xidx - 1, yidx - 1, xs, ys);
    maybe_check_square (xsq + yopsq,   xidx - 1, yidx + 1, xs, ya);
    maybe_check_square (xopsq + ysq,   xidx + 1, yidx - 1, xa, ys);
    maybe_check_square (xopsq + yopsq, xidx + 1, yidx + 1, xa, ya);
    
    return closest_dist;
}

static unsigned int check_cube (const struct vn_worley_generator *generator,
                                int sx, int sy, int sz,
                                int dx, int dy, int dz)
{
    unsigned int rnd = lolrand (sx, sy, sz, generator->seed);
    unsigned int ndots = generator->ndots;
    unsigned int i;

    unsigned int closest_dist = UINT_MAX;
    for (i=0; i<ndots; i++) {
        // Get 8-bit fixed point dot coordinates
        rnd = xorshift32 (rnd);
        unsigned int dotx = rnd & 0xff;
        unsigned int doty = (rnd >> 8) & 0xff;
        unsigned int dotz = (rnd >> 16) & 0xff;
        int x = dotx - dx;
        int y = doty - dy;
        int z = dotz - dz;
        unsigned int dist = x*x + y*y + z*z;
        closest_dist = (dist < closest_dist)? dist: closest_dist;
    }

    return closest_dist;
}

#define maybe_check_cube(dist, xidx, yidx, zidx, x, y, z) do {          \
        unsigned int dist2;                                             \
        if (dist < closest_dist) {                                      \
            dist2 = check_cube (generator, xidx, yidx, zidx, x, y, z);  \
            closest_dist = (dist2 < closest_dist)? dist2: closest_dist; \
        }                                                               \
    } while (0)
static unsigned int noise_3d (const struct vn_generator *gen, unsigned int x, unsigned int y, unsigned int z)
{
    struct vn_worley_generator *generator = (struct vn_worley_generator*)gen;

    unsigned int width = generator->width;
    unsigned int height = generator->height;
    unsigned int depth = generator->depth;

    unsigned int xidx = x >> width;
    unsigned int yidx = y >> height;
    unsigned int zidx = z >> depth;

    // Translate x,y local coordinates to 8-bit fixed point
    unsigned int xmask = (1 << width) - 1;
    unsigned int ymask = (1 << height) - 1;
    unsigned int zmask = (1 << depth) - 1;

    unsigned int xdiff = x & xmask;
    xdiff <<= 8;
    xdiff >>= width;

    unsigned int ydiff = y & ymask;
    ydiff <<= 8;
    ydiff >>= height;

    unsigned int zdiff = z & zmask;
    zdiff <<= 8;
    zdiff >>= depth;

    unsigned int closest_dist = check_cube (generator, xidx, yidx, zidx, xdiff, ydiff, zdiff);

    int xa, xs, ya, ys, za, zs;
    xa = xdiff - (1<<8);
    ya = ydiff - (1<<8);
    za = zdiff - (1<<8);
    xs = xdiff + (1<<8);
    ys = ydiff + (1<<8);
    zs = zdiff + (1<<8);

    unsigned int xsq, ysq, xopsq, yopsq, zsq, zopsq;
    xsq = xdiff*xdiff;
    ysq = ydiff*ydiff;
    zsq = zdiff*zdiff;
    xopsq = xa*xa;
    yopsq = ya*ya;
    zopsq = za*za;

    // Check adjacent squares
    maybe_check_cube (xsq,   xidx - 1, yidx, zidx, xs, ydiff, zdiff);
    maybe_check_cube (xopsq, xidx + 1, yidx, zidx, xa, ydiff, zdiff);
    maybe_check_cube (ysq,   xidx, yidx - 1, zidx, xdiff, ys, zdiff);
    maybe_check_cube (yopsq, xidx, yidx + 1, zidx, xdiff, ya, zdiff);
    maybe_check_cube (zsq,   xidx, yidx, zidx - 1, xdiff, ydiff, zs);
    maybe_check_cube (zopsq, xidx, yidx, zidx + 1, xdiff, ydiff, za);

    maybe_check_cube (xsq + ysq,     xidx - 1, yidx - 1, zidx, xs, ys, zdiff);
    maybe_check_cube (xsq + yopsq,   xidx - 1, yidx + 1, zidx, xs, ya, zdiff);
    maybe_check_cube (xopsq + ysq,   xidx + 1, yidx - 1, zidx, xa, ys, zdiff);
    maybe_check_cube (xopsq + yopsq, xidx + 1, yidx + 1, zidx, xa, ya, zdiff);

    maybe_check_cube (xsq + zsq, xidx - 1, yidx, zidx - 1, xs, ydiff, zs);
    maybe_check_cube (ysq + zsq, xidx, yidx - 1, zidx - 1, xdiff, ys, zs);
    maybe_check_cube (yopsq + zsq, xidx, yidx + 1, zidx - 1, xdiff, ya, zs);
    maybe_check_cube (xopsq + zsq, xidx + 1, yidx, zidx - 1, xa, ydiff, zs);

    maybe_check_cube (xsq + zopsq, xidx - 1, yidx, zidx + 1, xs, ydiff, za);
    maybe_check_cube (ysq + zopsq, xidx, yidx - 1, zidx + 1, xdiff, ys, za);
    maybe_check_cube (yopsq + zopsq, xidx, yidx + 1, zidx + 1, xdiff, ya, za);
    maybe_check_cube (xopsq + zopsq, xidx + 1, yidx, zidx + 1, xa, ydiff, za);

    maybe_check_cube (xsq + ysq + zsq, xidx - 1, yidx - 1, zidx - 1, xs, ys, zs);
    maybe_check_cube (xsq + yopsq + zsq, xidx - 1, yidx + 1, zidx - 1, xs, ya, zs);
    maybe_check_cube (xopsq + yopsq + zsq, xidx + 1, yidx + 1, zidx - 1, xa, ya, zs);
    maybe_check_cube (xopsq + ysq + zsq, xidx + 1, yidx - 1, zidx - 1, xa, ys, zs);
    maybe_check_cube (xsq + ysq + zopsq, xidx - 1, yidx - 1, zidx + 1, xs, ys, za);
    maybe_check_cube (xsq + yopsq + zopsq, xidx - 1, yidx + 1, zidx + 1, xs, ya, za);
    maybe_check_cube (xopsq + yopsq + zopsq, xidx + 1, yidx + 1, zidx + 1, xa, ya, za);
    maybe_check_cube (xopsq + ysq + zopsq, xidx + 1, yidx - 1, zidx + 1, xa, ys, za);

    return closest_dist;
}
