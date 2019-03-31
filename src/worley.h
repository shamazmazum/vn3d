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
   noise in the range [0; 2^32), though with 10-bit resolution only.

   \param dots A small number. Number of feature dots in cell of the
   grid will be in the range `[1, 2^dots]`.
   \param grid_pow Grid size is `2^grid_pow`.
   \return Created generator.
**/
struct vn_generator* vn_worley_generator (unsigned int dots, unsigned int grid_pow);
#endif
