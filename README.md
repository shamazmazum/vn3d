VN3D
====

**VN3D** is a simple C library for generating 2D/3D smooth value noise in range [0, 2^32-1]. It uses integer math only and can be well parallelized.

Examples:

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
