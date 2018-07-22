VN3D
====

**VN3D** is a simple C library for generating 2D/3D smooth value noise in range [0, 2^32-1]. It uses
integer math only and can be well parallelized.

To use it you must create a generator with call to `vn_make_generator()`. It accepts four arguments:
number of octaves and initial grid size. In the following example the grid size is `2^10 x 2^10 x
2^10` and the number of octaves is `5`:
~~~~{.c}
struct vn_generator *gen = vn_make_generator (5, 10, 10, 10);
~~~~

The initial grid size determines the lowest frequency in the output noise in three
dimensions. Bigger numbers for initial grid size result in lower frequency for the lowest frequency
component. If you want 2D noise, please specify `VN_SIZE_UNSPECIFIED` for the last argument. The
number of octaves specify how many higher frequency details will be mixed into the output. Bigger
number of octaves result in more detailed noise. As limitation, number of octaves must be less or
equal to `min (width, height, depth)`. It is a good idea for initial grid size to be close to size
of resulting texture. E.g. if you want to generate 2D texture with size `1000x1000` it's good to
call `vn_make_generator()` like so:
~~~~{.c}
gen = vn_make_generator (octaves, 10, 10, VN_SIZE_UNSPECIFIED);
~~~~

Then you call `vn_noise_3d()` or `vn_noise_2d()` in nested loops to generate a texture. The
generator `gen` is not modified during these calls, so loops can be parallelized, provided the
output of each iteration goes through different cache lines.

Finally, generator must be destroyed with `vn_destroy_generator()`.

Examples:
---------

~~~~{.c}
// Make a generator with ten octaves and lattice size 1024x1024x1024.
struct vn_generator *gen = vn_make_generator (5, 10, 10, 10);
// Get noise value in point (1, 2, 3).
unsigned int val = vn_noise_3d (gen, 1, 2, 3);
// Get noise value in point (10, 12) (2d).
unsigned int val = vn_noise_2d (gen, 10, 12);
// Free the generator after use
vn_destroy_generator (gen);
~~~~

Examples of generated textures (256x256 pixels):
-----------------------------------------------

Unfortunately, this section works only with doxygen generated documentation now :-(

![Noise with 3 octaves](octaves3.png)
![Noise with 8 octaves](octaves8.png)
