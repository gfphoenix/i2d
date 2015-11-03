#ifndef GLYPH_HPP
#define GLYPH_HPP
#include <stdint.h>
#include <Ref.hpp>
#include <Texture.hpp>

class Glyph
{
public:
    Glyph();
    inline int8_t xOffset()const{return xoffset_;}
    inline int8_t yOffset()const{return yoffset_;}
    inline int16_t xAdvance()const{return xadvance_;}
    inline int width()const{return width_;}
    inline int height()const{return height_;}
    inline uint16_t code()const{return unicode16_;}
    inline const Ref_ptr<TextureRegion2D> &getTextureRegion()const{return texture_;}
    void dump()const;
protected:
    friend class BMFont;
    Ref_ptr<TextureRegion2D> texture_;
    uint16_t unicode16_;
    uint16_t x_,y_;
    uint16_t width_, height_;
    int16_t xadvance_;
    int8_t xoffset_, yoffset_;
};

#endif // GLYPH_HPP
