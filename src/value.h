/**
   @file value.h
   @brief Value noise generator.
**/

#ifndef __VALUE_H__
#define __VALUE_H__

#include <limits.h>
#include "generic.h"

/**
   \brief Make a value noise generator.

   Affects the error code setting it to ALL_OK. Returned noise is in
   the range [0; 2^32-1]. All grid sizes are converted to max (\p
   octaves, grid_size), i.e. grid size cannot be less than number of
   octaves.

   \param octaves Number of high frequency components in the output. Must be <= min
          (width, height, depth).
   \param width Initial grid size for X axis is 2^width.
   \param height Initial grid size for Y axis is 2^height.
   \param depth Initial grid size for Z axis is 2^depth.
   \return Created generator.
**/
struct vn_generator* vn_value_generator (unsigned int octaves, unsigned int width,
                                         unsigned int height, unsigned int depth);

#endif
