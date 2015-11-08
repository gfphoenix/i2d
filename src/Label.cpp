#include "Label.hpp"
#include <Renderer.hpp>
#include <PrimitiveShader.hpp>
#include <SpriteShader.hpp>

Label::Label()
{
    setShader(SpriteShader::getInstance());
}
Label::~Label(){}
void Label::DrawSelf(Renderer *renderer)
{
    if(!bmSet_ || codes_.empty())
        return;
    auto bm = bmSet_.get();
    auto sh = static_cast<SpriteShader*>(getShader());

    int x = 0;
    int y = getHeight();
    auto const &T = getWorldTransform();
    Vec2 pos;
    uint16_t prev = 0;

    if(false)
    {
        auto shader = PrimitiveShader_PC::getInstance();

        renderer->Use(shader);
        Color4 ccs[3] = {Color4(1,0,0,.5f), Color4(0,1,0,.5f), Color4(0,0,1,.5f)};
        int i=0;
        shader->pushTransform(T);

        float ww = getWidth();
        float hh = getHeight();
        shader->pushColor(Color4(1,1,0,.7f));
        shader->drawRect(0, 0, ww, hh);
        shader->popColor();
        for(uint16_t v : codes_){
            auto g = bm->findGlyph(v);
            if(!g)
                continue;
            auto rr = g->getTextureRegion().get();
            pos.x = x + g->xOffset();
            pos.y = y - g->yOffset() - g->height();// rr->getRegionHeight();
            //sh->drawOffset(rr, pos);
//            sh->draw(T, rr, pos, rr->getRectSize());
            shader->pushColor(ccs[(i++)%3]);
            shader->drawRect(pos, rr->getRectSize());
            shader->popColor();
            x += g->xAdvance() + bm->findKerning(prev,g->code());
            prev = g->code();
        }
        shader->pushColor(Color4(1,1,1,1));
        shader->drawLine(Vec2(0, y), Vec2(getWidth(), y));
        auto lh = bmSet_->getLineHeight();
        auto sz = bmSet_->getFontSize();
        shader->pushColor(Color4(0,0,1,1));
        {
            shader->drawLine(Vec2(0, sz), Vec2(getWidth(), sz));
            auto g = bmSet_->findGlyph(codes_.at(0));
            if(g){
//                auto rr = g->getTextureRegion();
                auto tmp = y - g->yOffset();
                shader->pushColor(Color4(1,0,0,1));
                shader->drawLine(Vec2(0, tmp), Vec2(getWidth(), tmp));
                shader->popColor();
            }
        }
        shader->drawLine(Vec2(0, 0), Vec2(getWidth(), 0));
        shader->popColor();
        shader->drawLine(Vec2(0, y-lh), Vec2(getWidth(), y-lh));
        shader->popColor();

        shader->popTransform();

        renderer->Use(sh);
        x = 0;
        y = getHeight();
        prev = 0;
    }

    //sh->pushTransform(getWorldTransform());
    sh->pushColor(getColor());
    for(uint16_t v : codes_){
        auto g = bm->findGlyph(v);
        if(!g)
            continue;
        auto rr = g->getTextureRegion().get();
        pos.x = x + g->xOffset();
        pos.y = y - g->yOffset() - g->height();
        sh->draw(T, rr, pos, rr->getRectSize());
        x += g->xAdvance() + bm->findKerning(prev, g->code());
        prev = g->code();
    }
    sh->popColor();
    //sh->popTransform();
}

void Label::setString(const std::string &str)
{
    str_ = str;
    updateCodes();
    if(bmSet_)
        setWidth(calculateTextWidth());
}

void Label::setString(std::string &&str)
{
    str_ = std::move(str);
    updateCodes();
    if(bmSet_)
        setWidth(calculateTextWidth());
}

int Label::calculateTextWidth()const
{
    int w=0;
    auto bm = bmSet_.get();
    uint16_t prev = 0;
    for(uint16_t v : codes_){
        auto g = bm->findGlyph(v);
        if(g!=nullptr){
            w += g->xAdvance() + bm->findKerning(prev, g->code());
            prev = g->code();
        }
    }
    return w;
}
// 0x0000 - 0x007f  => 0b 0xxx xxxx
// 0x0080 - 0x07ff  => 0b 110x xxxx 10xx xxxx
// 0x0800 - 0xffff  => 0b 1110 xxxx 10xx xxxx 10xx xxxx
static inline bool isB1(uint8_t c){return (c & 0x80) == 0; }
static inline bool isB2(uint8_t c){return (c & 0xe0) == 0xc0;}
static inline bool isB3(uint8_t c){return (c & 0xf0) == 0xe0;}
static inline bool isMB(uint8_t c){return (c & 0xc0) == 0x80;}
void Label::updateCodes()
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
