VN3D
====

[![Build Status](https://travis-ci.com/shamazmazum/vn3d.svg?branch=master)](https://travis-ci.com/shamazmazum/vn3d)

**VN3D** is a simple C library for generating 2D/3D smooth value noise in range [0, 2^32-1]. It uses
integer math only and can be well parallelized.

To use it you must create a generator with call to `vn_value_generator()`. It accepts two arguments:
number of octaves and initial grid size. In the following example the uniform `2^10 x 2^10 x 2^10`
grid is created and the number of octaves is `5`:
~~~~{.c}
struct vn_generator *gen = vn_value_generator (5, 10);
~~~~

The initial grid size determines the lowest frequency in the output noise. Bigger number for
initial grid size results in lower frequency for the lowest frequency component. The number of
octaves determines how many higher frequency details will be mixed into the output. Bigger number of
octaves results in more detailed noise. As a limitation, grid size values cannot be less than number
of octaves. It is a good idea for initial grid size to be close to `n-1` or `n-2` where `2^n` is
length, width or depth of a texture you want to generate. E.g. if you want to generate 2D texture
with size `1000x1000` it's good to call `vn_value_generator()` like so:
~~~~{.c}
gen = vn_value_generator (octaves, 8);
~~~~

Then you call `vn_noise_3d()`, `vn_noise_2d()` or `vn_noise_1d()` in nested loops to generate a
texture. The generator `gen` is not modified during these calls, so loops can be parallelized,
provided the output of each iteration goes through different cache lines.

Finally, generator must be destroyed with `vn_destroy_generator()`.

Examples:
---------

~~~~{.c}
// Make a generator with five octaves and lattice size 256x256x256.
struct vn_generator *gen = vn_value_generator (5, 8);
// Get noise value in point (1, 2, 3).
unsigned int val = vn_noise_3d (gen, 1, 2, 3);
// Get noise value in point (10, 12) (2d).
unsigned int val = vn_noise_2d (gen, 10, 12);
// Free the generator after use
vn_destroy_generator (gen);
~~~~

Examples of generated value noise textures (256x256 pixels):
-----------------------------------------------

|   1   |    2   |   3    |
|-------|--------|--------|
![4 octaves, grid size 16x16][img1] | ![3 octaves, grid size 64x64][img2] | ![6 octaves, grid size 64x64][img3]

[img1]: doc/octaves4-16x16.jpg
[img2]: doc/octaves3-64x64.jpg
[img3]: doc/octaves6-64x64.jpg

Examples of generated worley noise textures (256x256 pixels):
------------------------------------------------------------

|   1   |   2  |   3  |
|-------|------|------|
![1 dot, grid size 32x32][img4] | ![2 dots, grid size 32x32][img5] | ![1 dot, grid size 64x64][img6]

[img4]: doc/worley1-32x32.jpg
[img5]: doc/worley2-32x32.jpg
[img6]: doc/worley1-64x64.jpg

New in version 0.3:
-------------------
Worley noise was added and API wad changed a bit. See `vn_worley_generator()`.

New in version 0.4:
-------------------
1D value noise.
Value noise generator constructor always succeeds.

New in version 0.5:
-------------------
Fixed build on linux.
Simple example program called **vn3dgen** which generates grayscale
jpeg noise textures using **jpeg-turbo** library.
