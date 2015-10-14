#include "glyph.hpp"
#include <stdio.h>
Glyph::Glyph()
{
}

void Glyph::dump()const
{
    printf("code=%d|0x%x, x=%d, y=%d, w=%d, h=%d, bearingX=%d, bearingY=%d, xadvance=%d\n",
           code(), code(), (int)x_, (int)y_, (int)width_,
           (int)height_, (int)bearingX_, (int)bearingY_,
           (int)xadvance_);
}
