#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#define VN_GENERATOR_METHODS void (*destroy_generator) (struct vn_generator*); \
    unsigned int (*noise_2d) (const struct vn_generator*, unsigned int, unsigned int); \
    unsigned int (*noise_3d) (const struct vn_generator*, unsigned int, unsigned int, unsigned int);

struct vn_generator {
    VN_GENERATOR_METHODS
};

#endif
