#include "Glyph.hpp"
#include <Log.hpp>
Glyph::Glyph()
{
}

void Glyph::dump()const
{
    Log::i("code=%d|0x%x, x=%d, y=%d, w=%d, h=%d, bearingX=%d, bearingY=%d, xadvance=%d\n",
           code(), code(), (int)x_, (int)y_, (int)width_,
           (int)height_, (int)xoffset_, (int)yoffset_,
           (int)xadvance_);
}
