/**
   @file generic.h
   @brief General stuff of noise library.
**/

#ifndef __GENERIC_H__
#define __GENERIC_H__

/**
   \brief Noise generator structure.
**/
struct vn_generator;

/**
   \brief Destroy a noise generator.
**/
void vn_destroy_generator (struct vn_generator *generator);

/**
   \brief Get a noise value at the point `(x, y, z)`.
**/
unsigned int vn_noise_3d (const struct vn_generator *generator, unsigned int x,
                          unsigned int y, unsigned int z);

/**
   \brief Get a noise value at the point `(x, y)`.
**/
unsigned int vn_noise_2d (const struct vn_generator *generator, unsigned int x, unsigned int y);

/**
   \brief Get a noise value at the point `(x)`.
**/
unsigned int vn_noise_1d (const struct vn_generator *generator, unsigned int x);

/**
   \brief Error codes.
**/
enum vn_errcode {
    ALL_OK, /**< No error. **/
};

/**
   \brief Global error code.
**/
extern enum vn_errcode vn_errcode;

/**
   \brief Get the last error code.
**/
enum vn_errcode vn_get_error();

/**
   \brief Get the last error message.
**/
const char* vn_get_error_msg();

#endif
