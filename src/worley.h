/**
   @file worley.h
   @brief Worley noise generator.
**/

#ifndef __WORLEY_H__
#define __WORLEY_H__
#include "generic.h"

/**
   \brief Make worley noise generator.

   This function affects the error code setting it to ALL_OK. Returns
   noise in the range [0; 2^32-1], though with 10-bit resolution only.

   \param dots A small number. Number of feature dots in cell of the
   grid will be in the range [1, 2^dots].
   \param width Grid size for X axis is 2^width.
   \param height Grid size for Y axis is 2^height.
   \param depth Grid size for Z axis is 2^depth.
   \return Created generator.
**/
struct vn_generator* vn_worley_generator (unsigned int dots, unsigned int width,
                                          unsigned int height, unsigned int depth);
#endif
