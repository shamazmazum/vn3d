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
   noise in the range up to [0; 2^17], though with 8-bit resolution
   only.

   \param ndots Number of feature dots in cell of the grid.
   \param width Grid size for X axis is 2^width.
   \param height Grid size for Y axis is 2^height.
   \param depth Grid size for Z axis is 2^depth. Pass any value
          (e.g. `VN_SIZE_UNSPECIFIED`) for 2D noise.
   \return Created generator.
**/
struct vn_generator* vn_worley_generator (unsigned int ndots, unsigned int width,
                                          unsigned int height, unsigned int depth);
#endif
