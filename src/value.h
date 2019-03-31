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

   Affects the error code setting it to `ALL_OK`. Returned noise is in
   the range `[0; 2^32)`. Grid size is converted to `max (octaves,
   grid_pow)`, i.e. grid size power cannot be less than number of
   octaves.

   \param octaves Number of high frequency components in the
          output.
   \param grid_pow Initial grid size is `2^grid_pow`. Actual grid_pow is
          `max (octaves, grid_pow)`.
   \return Created generator.
**/
struct vn_generator* vn_value_generator (unsigned int octaves, unsigned int grid_pow);

#endif
