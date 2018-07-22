/**
   @file vn3d.h
   @brief Value noise generator.
**/

#ifndef __VN3D__
#define __VN3D__

#include <limits.h>

/**
   \brief Value to use for the last argument of `vn_make_generator()` in the case of 2D
   noise.
**/
#define VN_SIZE_UNSPECIFIED UINT_MAX

/**
   \brief Noise generator structure.
**/
struct vn_generator;

/**
   \brief Make a noise generator.

   Affects the error code.

   \param octaves Number of high frequency components in the output. Must be <= min
          (width, height, depth).
   \param width Initial grid size for X axis is 2^width.
   \param height Initial grid size for Y axis is 2^height.
   \param depth Initial grid size for Z axis is 2^depth. Pass `VN_SIZE_UNSPECIFIED` for 2D
          noise.
   \return Created generator or NULL in the case of error.
**/
struct vn_generator* vn_make_generator (unsigned int octaves, unsigned int width,
                                        unsigned int height, unsigned int depth);

/**
   \brief Destroy a noise generator.
**/
void vn_destroy_generator (struct vn_generator *generator);

/**
   \brief Get a noise value at the point `(x, y, z)`.

   \return Value in the range `[0, 2^32 - 1]`.
**/
unsigned int vn_noise_3d (const struct vn_generator *generator, unsigned int x,
                          unsigned int y, unsigned int z);

/**
   \brief Get a noise value at the point `(x, y)`.

   \return Value in the range `[0, 2^32 - 1]`.
**/
unsigned int vn_noise_2d (const struct vn_generator *generator, unsigned int x, unsigned int y);

/**
   \brief Error codes.
**/
enum vn_errcode {
    ALL_OK, /**< No error. **/
    TOO_MANY_OCTAVES /**< Number of octaves is too big. */
};

/**
   \brief Get the last error code.
**/
enum vn_errcode vn_get_error();

/**
   \brief Get the last error message.
**/
const char* vn_get_error_msg();

#endif
