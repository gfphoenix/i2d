#include "BMFont.hpp"
#include <File.hpp>
#include <Log.hpp>
#include <Glyph.hpp>
#include <mm.hpp>

#include <string.h>
#include <vector>

BMFont::BMFont(){}

void BMFont::dump()const
{
    Log::i("face=\"%s\", size=%d, padding=%d,%d,%d,%d, space=%d,%d",
           fontName_.c_str(), (int)size_,
           (int)padding_[0], (int)padding_[1],
           (int)padding_[2], (int)padding_[3],
            (int)space_[0], (int)space_[1]);
    Log::i("lineHeight=%d, base=%d, texture file name=\"%s\", glyph count=%d",
           (int)lineHeight_, (int)base_, textureFileName_.c_str(), (int)charmap_.size());

}

void BMFont::addGlyph(const Glyph &g)
{
    if(charmap_.find(g.code())!=charmap_.end()){
        Log::d("Glyph(code=0x%x) exists already", g.code());
        return ;
    }
    charmap_.insert({g.code(), g});
}

const Glyph *BMFont::findGlyph(uint16_t ucs16)const
{
    auto x = charmap_.find(ucs16);
    return x==charmap_.end() ? nullptr : &x->second;
}

int BMFont::findKerning(uint16_t first, uint16_t second)const
{
    auto v = makeup32(first, second);
    auto x = kerning_.find(v);
    return x==kerning_.end() ? 0 : x->second;
}

void BMFont::addKerning(uint16_t first, uint16_t second, int amount)
{
    auto v = makeup32(first, second);
    if(kerning_.find(v)!=kerning_.end()){
        Log::d("kerning(0x%x, 0x%x) exists already", first, second);
        return;
    }
    kerning_.insert({v,amount});
}

// copy `"xxyy"` to buffer with size=n
static int parseString(const char *data, char *buffer, int n)
{
    int i=0;
    char *p=buffer;
    if(data==nullptr || *data!='"' || --n<=0)
        goto out;
    data++;

    while(*data!='\0' && *data!='"' && i < n){
        *p++ = *data++;
        i++;
    }
    if(*data!='"')
        i=0;

out:
    buffer[i] = '\0';
    return i;
}

bool BMFont::parseInfo(BMFont &out, const char *p)
{
    const char *q;
    char tmp[128];
    if(p==nullptr || !(p[0]=='i' && p[1]=='n' && p[2]=='f' && p[3]=='o'))
        return false;
    // face
    q = strstr(p, "face=");
    if(q==nullptr)
        return false;
    q+=5;
    q+=parseString(q, tmp, sizeof(tmp));
    out.fontName_ = tmp;
    p=q;

    // size;
    q = strstr(p, "size=");
    if(q==nullptr)
        return false;
    q+=5;
    out.size_ = strtol(q, nullptr, 10);
    p=q;

    // padding
    q = strstr(p, "padding=");
    q+=8;
    {
        int padding[4];
        sscanf(q, "%d,%d,%d,%d",
               padding, padding+1, padding+2, padding+3);
        out.padding_[0] = padding[0];
        out.padding_[1] = padding[1];
        out.padding_[2] = padding[2];
        out.padding_[3] = padding[3];
    }
    p=q;

    // spacing
    q = strstr(p, "spacing=");
    if(q==nullptr)
        return false;
    q+=8;
    {
        int spacing[2];
        sscanf(q, "%d,%d", spacing, spacing+1);
        out.space_[0] = spacing[0];
        out.space_[1] = spacing[1];
    }
    return true;
}

bool BMFont::parseCommon(BMFont &out, const char *p)
{
    const char *q;
    if(p==nullptr || memcmp(p, "common", 6))
        return false;
    p+= 7;

    // line-height
    q = strstr(p, "lineHeight=");
    if(q==nullptr)
        return false;
    q+=11;
    out.lineHeight_ = strtol(q, nullptr, 10);
    p=q;

    // base
    q = strstr(p, "base=");
    if(q==nullptr)
        return false;
    q+=5;
    out.base_ = strtol(q, nullptr, 10);
    p=q;

    // check page
    q = strstr(p, "pages=");
    if(q==nullptr)
        return false;
    q+=6;
    int n = strtol(q, nullptr, 10);
    if(n>1){
        Log::e("Pages=%d > 1 is not supported...", n);
        return false;
    }
    return true;
}

bool BMFont::parsePage(BMFont &out, const char *p)
{
    const char *q;
    if(p==nullptr || memcmp(p, "page", 4))
        return false;
    p+=4;
    q = strstr(p, "file=");
    if(q==nullptr)
        return false;
    q+= 5;
    char tmp[128];
    parseString(q, tmp, sizeof(tmp));
    out.textureFileName_ = tmp;
    return out.textureFileName_.size()>0;
}

static int parseChars(const char *p)
{
    if(p==nullptr || memcmp(p, "chars", 5))
        return 0;
    p+=5;
    p = strstr(p, "count=");
    if(p==nullptr)
        return 0;
    p+=6;
    int n = strtol(p, nullptr, 10);
    return n;
}

bool BMFont::parseChar(BMFont &out, const char *p)
{
    if(p==nullptr || memcmp(p, "char", 4))
        return false;
    p+=4;
    Glyph g;
    int id, x, y, w, h, offx, offy, xadvance;
    // id
    p = strstr(p, "id=");
    if(p==nullptr)
        return false;
    p+=3;
    id = strtol(p, nullptr, 10);

    // x,y
    p = strstr(p, "x=");
    if(p==nullptr)
        return false;
    p+=2;
    x = strtol(p, nullptr, 10);

    p = strstr(p, "y=");
    if(p==nullptr)
        return false;
    p+=2;
    y = strtol(p, nullptr, 10);

    // w, h
    p = strstr(p, "width=");
    if(p==nullptr)
        return false;
    p+=6;
    w = strtol(p, nullptr, 10);

    p = strstr(p, "height=");
    if(p==nullptr)
        return false;
    p+=7;
    h = strtol(p, nullptr, 10);

    // offx, offy
    p = strstr(p, "xoffset=");
    if(p==nullptr)
        return false;
    p+=8;
    offx = strtol(p, nullptr, 10);

    p = strstr(p, "yoffset=");
    if(p==nullptr)
        return false;
    p+=8;
    offy = strtol(p, nullptr, 10);

    // xadvance
    p = strstr(p, "xadvance=");
    if(p==nullptr)
        return false;
    p+=9;
    xadvance = strtol(p, nullptr, 10);

    {
        // check
        if(!checkUCS16(id)){
            Log::e("id is tooo large or <=0 : %d", (int)id);
            return false;
        }
        g.unicode16_ = (uint16_t)id;
        g.x_            = x;
        g.y_            = y;
        g.width_        = w;
        g.height_       = h;
        g.xoffset_     = offx;
        g.yoffset_     = offy;
        g.xadvance_     = xadvance;
        g.texture_      = out.texture_->getTextureRegion(x, y, w, h, RegionDirection::BOTTOM);
    }
    out.addGlyph(g);
    return true;
}

static int parseKernings(const char *p)
{
    if(p==nullptr || memcmp(p, "kernings", 8))
        return 0;
    p+=8;
    p = strstr(p, "count=");
    if(p==nullptr)
        return 0;
    p+=6;
    return strtol(p, nullptr, 10);
}

bool BMFont::parseKerning(BMFont &out, const char *p)
{
    long first, second, amount;
    if(p==nullptr || memcmp(p, "kerning", 7))
        return false;
    p+=8;
    // first
    p = strstr(p, "first=");
    if(p==nullptr)
        return false;
    p+=6;
    first = strtol(p, nullptr, 10);
    // second
    p = strstr(p, "second=");
    if(p==nullptr)
        return false;
    p+=7;
    second = strtol(p, nullptr, 10);
    // amount
    p = strstr(p, "amount=");
    if(p==nullptr)
        return false;
    p+=7;
    amount = strtol(p, nullptr, 10);
    {
        // check
        if(!checkUCS16(first) || !checkUCS16(second)){
            Log::e("bad first or second code value(%d,%d)",
                    (int)first, (int)second);
            return false;
        }
    }
    out.addKerning((uint16_t)first, (uint16_t)second, amount);
    return true;
}
BMFont *BMFont::load(const Buffer &data)
{
    LineReader lr(data);
    unsigned char buffer[1024];
    char *p=(char *)buffer;
    BMFont *tmpObj = MM<BMFont>::New();
    BMFont &TMP = *tmpObj;
    BMFont *out=nullptr;
    int n;
    // parse info
    //p = fgets(buffer, sizeof(buffer), file);
    lr.readLine(buffer, sizeof(buffer));
    if(!parseInfo(TMP, p))
        goto out;
    // common
    //p = fgets(buffer, sizeof(buffer), file);
    lr.readLine(buffer, sizeof(buffer));
    if(!parseCommon(TMP, p))
        goto out;
    // page
    //p = fgets(buffer, sizeof(buffer), file);
    lr.readLine(buffer, sizeof(buffer));
    if(!parsePage(TMP, p))
        goto out;
    {// load texture
        Log::d("file=<%s>\n", TMP.textureFileName_.c_str());
        TMP.texture_ = TextureManager::getInstance()->loadTexture(TMP.textureFileName_);
        Log::d("tex=%p\n", TMP.texture_.get());
    }
    // chars
    //p = fgets(buffer, sizeof(buffer), file);
    lr.readLine(buffer, sizeof(buffer));
    n = parseChars(p);
    if(n<=0)
        goto out;
    while(n-->0){
        //p = fgets(buffer, sizeof(buffer), file);
        lr.readLine(buffer, sizeof(buffer));
        if(!parseChar(TMP, p))
            goto out;
    }
    // special care
    //p = fgets(buffer, sizeof(buffer), file);
    lr.readLine(buffer, sizeof(buffer));
    if(parseChar(TMP, p)){
    // kernings, optional
        //p = fgets(buffer, sizeof(buffer), file);
        lr.readLine(buffer, sizeof(buffer));
    }
    n = parseKernings(p);
    while(n-->0){
        //p = fgets(buffer, sizeof(buffer), file);
        lr.readLine(buffer, sizeof(buffer));
        if(!parseKerning(TMP, p))
            //goto out;
            break;
    }

    out = tmpObj;
    tmpObj = nullptr;
    out->dump();
out:
    MM<BMFont>::Del(tmpObj);
    return out;
}

BMFont *BMFont::load(const char *fnt)
{
    auto file = FileUtils::Open(fnt);
    if(!file)
        return nullptr;
    BMFont *result=nullptr;
    auto len = file->Length();
    if(len<(100UL<<10)){
        unsigned char tmp[len];
        auto n = file->Read(tmp, sizeof(tmp));
        if(n<(int)len)
            return nullptr;
        Buffer b;
        b.addr = tmp;
        b.size = sizeof(tmp);
        result = load(b);
    }else{
        BufferA b(len);
        auto n = file->Read(b.addr, b.size);
        if(n<(int)len)
            return nullptr;
        result = load(b);
    }
    return result;
}

void BMFont::Dispose(){}

void BMFont::test()
{
//    Ref_ptr<BMFont> f = load("font.fnt");
    Ref_ptr<BMFont> f = TextureManager::getInstance()->loadBMFontSet("font.fnt");
    if(f){
        f->dump();
    }else{
        Log::d("nil BMFont");
    }
    Log::d("count = %d", f->getTexture2D()->UseCount());
    {
    Log::d("&&&&&&&&&&&&&&&&&  f=%p, tex=%p", f.get(), f->getTexture2D().get());
    auto bm = TextureManager::getInstance()->loadBMFontSet("font.fnt");
    Log::d("fnt = %p, %s", bm.get(), bm->getFontName().c_str());
    Log::d("******************");
    Log::d("count = %d, %d", f->getTexture2D()->UseCount(), bm->getTexture2D()->UseCount());
    }
    Log::d("count = %d", f->getTexture2D()->UseCount());
}
