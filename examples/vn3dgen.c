#include <vn3d.h>
#include <turbojpeg.h>

#include <fcntl.h>
#include <unistd.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 256

static void usage()
{
    fprintf (stderr, "Usage:\n");
    fprintf (stderr, "vn3dgen output width height value <octaves> <grid_size>\n");
    fprintf (stderr, "vn3dgen output width height worley <dots> <grid_size>\n");

    exit(1);
}

int main (int argc, char *argv[])
{
    struct vn_generator *gen = NULL;
    int i,j,p = 0;
    unsigned char *buffer = NULL;
    tjhandle tjinstance;
    unsigned char *image = NULL;
    size_t imagesize;
    int fd;
    int width, height;

    if (argc < 5) usage ();

    width = strtol (argv[2], NULL, 10);
    height = strtol (argv[3], NULL, 10);
    if (width <= 0 || height <= 0) usage();
    srand (time (NULL));

    if (strcmp (argv[4], "value") == 0) {
        if (argc != 7) usage();
        int octaves = strtol (argv[5], NULL, 10);
        int gsize = strtol (argv[6], NULL, 10);
        if (octaves <= 0 || gsize <= 0) usage();
        gen = vn_value_generator (octaves, gsize);
    } else if (strcmp (argv[4], "worley") == 0) {
        if (argc != 7) usage();
        int dots = strtol (argv[5], NULL, 10);
        int gsize = strtol (argv[6], NULL, 10);
        if (dots <= 0 || gsize <= 0) usage();
        gen = vn_worley_generator (dots, gsize);
    } else usage();

    if (gen == NULL) {
        fprintf (stderr, "Cannot create generator\n");
        goto cleanup;
    }

    buffer = malloc (width*height*sizeof (char));
    for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            /* Generate 8 bit noise */
            buffer[p] = vn_noise_2d (gen, i, j) >> 24;
            p++;
        }
    }

    tjinstance = tjInitCompress();
    if (tjinstance == NULL) {
        fprintf (stderr, "Cannot create jpeg compressor\n");
        goto cleanup;
    }

    if (tjCompress2 (tjinstance, buffer, width, 0, height, TJPF_GRAY,
                     &image, &imagesize, TJSAMP_GRAY, 90, TJFLAG_PROGRESSIVE) != 0) {
        fprintf (stderr, "Cannot encode jpeg image: %s\n", tjGetErrorStr2 (tjinstance));
        goto cleanup;
    }

    fd = open (argv[1], O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror ("Cannot open file");
        goto cleanup;
    }

    write (fd, image, imagesize);
    close (fd);

cleanup:
    if (tjinstance != NULL) tjDestroy (tjinstance);
    if (gen != NULL) vn_destroy_generator (gen);
    free (image);
    free (buffer);

    return 0;
}
