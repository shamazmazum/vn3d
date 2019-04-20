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
   noise in the range [0; 2^32).

   \param dots A small number. Number of feature dots in a cell of the
   grid will be in the range `[1, 2^dots]`. Acceptable values are from
   0 to 4 meaning up to 16 dots in the cell. Bigger numbers will be
   clamped to the range [0,4].
   \param grid_pow Grid size is `2^grid_pow`.
   \return Created generator.
**/
struct vn_generator* vn_worley_generator (unsigned int dots, unsigned int grid_pow);
#endif
