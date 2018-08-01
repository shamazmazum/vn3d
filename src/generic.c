#include <stdlib.h>
#include "generic.h"
#include "private.h"

enum vn_errcode vn_errcode;
const struct error_mapping {
    enum vn_errcode errcode;
    const char *errmsg;
} error_mappings[] = {
    {ALL_OK, "No errors occured"},
    {TOO_MANY_OCTAVES, "Number of octaves must be equal or less than min (depth, width, height)"},
    {0, NULL}
};

static const char* find_error_msg (enum vn_errcode code)
{
    const struct error_mapping *ptr = &(error_mappings[0]);
    while (ptr->errcode != code && ptr->errmsg != NULL) ptr++;
    return (ptr->errmsg != NULL)? ptr->errmsg: "Unknown error";
}

enum vn_errcode vn_get_error ()
{
    return vn_errcode;
}

const char* vn_get_error_msg ()
{
    return find_error_msg (vn_errcode);
}

void vn_destroy_generator (struct vn_generator *generator)
{
    generator->destroy_generator (generator);
}

unsigned int vn_noise_2d (const struct vn_generator *generator, unsigned int x, unsigned int y)
{
    return generator->noise_2d (generator, x, y);
}

unsigned int vn_noise_3d (const struct vn_generator *generator, unsigned int x, unsigned int y, unsigned int z)
{
    return generator->noise_3d (generator, x, y, z);
}
