/* stb_impl.c -- single translation unit that instantiates the stb_image and
   stb_truetype implementations. See https://github.com/nothings/stb.
   Mirrors src/lua/minilua/minilua.c's single-header-implementation pattern. */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
