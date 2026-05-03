#pragma once

#include <stdint.h>

/* 5x5 graduation cap (downsampled from 15x15 image using subpixel rendering)
   Each value 0-100 represents brightness/gray level
   25 bytes per frame (5x5) - row-major order, same format as video frames
   Generated from: grad_cap/image.png (15x15, inverted) using 3x3 block averaging
*/

const uint8_t graduation_cap_frame[25] = {
      0,      0,     46,      0,      0,
     21,     78,     83,    100,     19,
      0,     37,     78,     59,      0,
      0,     22,     86,      9,      0,
      0,      0,      0,      0,      0
};
