#ifndef __VN3D__
#define __VN3D__

struct vn_generator;

struct vn_generator* vn_make_generator (unsigned int octaves, unsigned int width,
                                        unsigned int height, unsigned int depth);
void vn_destroy_generator (struct vn_generator *generator);
unsigned int vn_noise_3d (const struct vn_generator *generator, unsigned int x,
                          unsigned int y, unsigned int z);
unsigned int vn_noise_2d (const struct vn_generator *generator, unsigned int x, unsigned int y);

enum vn_errcode {
    TOO_MANY_OCTAVES
};

enum vn_errcode vn_get_error();
const char* vn_get_error_msg();

#endif
