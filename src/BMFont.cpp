#include "BMFont.hpp"
#include <Renderer.hpp>
#include <SpriteShader.hpp>

BMFont::BMFont()
{
    setShader(SpriteShader::getInstance());
}
BMFont::~BMFont(){}
void BMFont::DrawSelf(Renderer *renderer)
{
    if(!bmSet_ || codes_.empty())
        return;
    auto bm = bmSet_.get();
    auto sh = static_cast<SpriteShader*>(getShader());
    renderer->Use(sh);
    Vec2 pos;
    int x = 0, y=bm->getBase();

    auto const &T = getWorldTransform();
    //sh->pushTransform(getWorldTransform());
    sh->pushColor(getColor());

    for(uint16_t v : codes_){
        auto g = bm->findGlyph(v);
        if(!g)
            continue;
        auto rr = g->getTextureRegion().get();
        pos.x = x + g->xOffset();
        pos.y = y - g->yOffset() - g->height();// rr->getRegionHeight();
        //sh->drawOffset(rr, pos);
        sh->draw(T, rr, pos, rr->getRectSize());
        x += g->xAdvance();
    }
    sh->popColor();
    //sh->popTransform();
}

void BMFont::setString(const std::string &str)
{
    str_ = str;
    updateCodes();
}

void BMFont::setString(std::string &&str)
{
    str_ = std::move(str);
    updateCodes();
}

// 0x0000 - 0x007f  => 0b 0xxx xxxx
// 0x0080 - 0x07ff  => 0b 110x xxxx 10xx xxxx
// 0x0800 - 0xffff  => 0b 1110 xxxx 10xx xxxx 10xx xxxx
static inline bool isB1(uint8_t c){return (c & 0x80) == 0; }
static inline bool isB2(uint8_t c){return (c & 0xe0) == 0xc0;}
static inline bool isB3(uint8_t c){return (c & 0xf0) == 0xe0;}
static inline bool isMB(uint8_t c){return (c & 0xc0) == 0x80;}
void BMFont::updateCodes()
{
    auto p = (const uint8_t*)(str_.c_str());
    auto len = str_.size();
    codes_.clear();
    while(len>0){
        if(isB1(*p)){ //0x00-0x7f
            codes_.push_back((uint16_t)(*p++));
            len--;
        }else if(isB2(*p)){
            if(len>1 && isMB(p[1])){
                uint16_t a = 0x1f & (*p);
                uint16_t b = 0x3f & p[1];
                uint16_t v = (a<<6) | b;
                codes_.push_back(v);
            }
            p += 2;
            len -=2;
        }else if(isB3(*p)){
            if(len>2 && isMB(p[1]) && isMB(p[2])){
                uint16_t a = (*p) & 0x0f;
                uint16_t b = p[1] & 0x3f;
                uint16_t c = p[2] & 0x3f;
                uint16_t v = (a<<12) | (b<<6) | c;
                codes_.push_back(v);
            }
            p += 3;
            len -=3;
        }else{ // skip one bad byte
            p++;
            len--;
        }
    }
}
