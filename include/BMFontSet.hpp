#ifndef BMFONTSET_HPP
#define BMFONTSET_HPP
#include "glyph.hpp"
#include "Ref.hpp"
#include "ResourceManager.hpp"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <stdint.h>
class BMFontSet : public Resource
{
public:
    static inline uint32_t makeup32(uint16_t first, uint16_t second)
    {
        return (((uint32_t)first)<<16) | ((uint32_t)second);
    }
    static inline uint16_t extract0(uint32_t v)
    {
        return (uint16_t)(v>>16);
    }
    static inline uint16_t extract1(uint32_t v)
    {
        return (uint16_t)(v&0x0000ffff);
    }
    static inline bool checkUCS16(long id)
    {
        return id>0 && id<((1L)<<16);
    }

    static BMFontSet *load(const char *fnt);
    static void test();
    virtual void Dispose()override;
    BMFontSet();
    void dump()const;
    inline const std::string &getFontName()const{return fontName_;}
    inline const std::string &getFontFileName()const{return textureFileName_;}
    inline const Ref_ptr<Texture2D> &getTexture2D()const{return texture_;}
    inline int getFontSize()const{return size_;}
    inline int getLineHeight()const{return lineHeight_;}
    inline int getBase()const{return base_;}
    enum class PADDING{
        LEFT,RIGHT,TOP,BOTTOM,
    };
    enum class SPACE{
        LEFT,RIGHT,
    };

    inline int getPadding(PADDING idx)const
    {
        int i = (int)idx;
        //if(i<0 || i>3)
        //    return 0;
        return padding_[i];
    }
    inline int getSpace(SPACE idx)const
    {
        return space_[(int)idx];
    }
    int getStringWidth(const char *utf8String)const;
    const Glyph *findGlyph(uint16_t ucs16)const;
    int findKerning(uint16_t first, uint16_t second)const;

protected:
    static bool parseChar(BMFontSet &out, const char *p);
    static bool parseCommon(BMFontSet &out, const char *p);
    static bool parseInfo(BMFontSet &out, const char *p);
    static bool parseKerning(BMFontSet &out, const char *p);
    static bool parsePage(BMFontSet &out, const char *p);
    void addGlyph(const Glyph &g);
    void addKerning(uint16_t first, uint16_t second, int amount);
    Ref_ptr<Texture2D> texture_;
    std::string fontName_;
    std::string textureFileName_;

    std::map<uint16_t, Glyph> charmap_;
//    std::unordered_map<uint16_t, Glyph> charmap_;
    std::unordered_map<uint16_t, int> kerning_;
//    std::vector<Glyph> charset_;
//    std::vector<std::pair<uint32_t, int>> kerning_;

    int size_;
    int lineHeight_;
    int base_;
    union{
    int8_t padding_[4];
    struct{
    int8_t padding_L_;
    int8_t padding_R_;
    int8_t padding_T_;
    int8_t padding_B_;
    };
    };
    int8_t space_[2];
};

#endif // BMFONTSET_HPP
