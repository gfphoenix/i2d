#include <Renderer.hpp>
#include <Scene.hpp>
#include <Sprite.hpp>
#include <SpriteShader.hpp>
#include <string.h>

BaseSprite::BaseSprite(const Ref_ptr<TextureRegion2D> &region)
    :Node(), region_(region)
{
    initShader();
    if(region)
        setSize(region->getRegionSize());
}
void BaseSprite::initShader()
{
    auto sh = SpriteShader::getInstance();
    this->setShader(sh);
}

void BaseSprite::setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize)
{
    region_=region;
    if(region && resetSize)
        setSize(region->getRegionSize());
}
void BaseSprite::DrawSelf(Renderer *renderer)
{
    auto region = region_.get();
    if(!region)
        return;
    auto shader = static_cast<SpriteShader*>(getShader());
    renderer->Use(shader);
    shader->pushColor(getColor());
    shader->drawSize(getWorldTransform(), region, getSize());
    shader->popColor();
}
LightSprite::LightSprite(const Ref_ptr<TextureRegion2D> &region)
    :BaseSprite(region)
{
    auto r = region.get();
    if(r)
        uv = r->getUV();
}
void LightSprite::setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize)
{
    if(region!=region_ && region)
        uv = region->getUV();
    BaseSprite::setTextureRegion(region, resetSize);
}
void LightSprite::DrawSelf(Renderer *renderer)
{
    auto r = region_.get();
    if(!r)
        return;
    auto sh = static_cast<SpriteShader*>(getShader());
    renderer->Use(sh);
    sh->pushColor(getColor());
    sh->drawUV(getWorldTransform(), r->getTexture2D().get(), uv, getSize());
    sh->popColor();
}

Sprite::~Sprite(){}
Sprite::Sprite(const Ref_ptr<TextureRegion2D> &region) : LightSprite(region)
  , flipX_(false)
  , flipY_(false)
{}
Sprite::Sprite() :LightSprite()
    , flipX_(false)
    , flipY_(false)
{}

void Sprite::setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize)
{
    auto tmp = region!=region_ && region;
    if(tmp)
        uv = region->getUV();
    LightSprite::setTextureRegion(region, resetSize);
    if(tmp)
        updateFlip__();
}
#if 0
void Sprite::DrawSelf(Renderer *renderer)
{
    if(!region_)
        return;
    auto shader = static_cast<SpriteShader*>(getShader());
    renderer->Use(shader);
    shader->pushColor(getColor());
    shader->drawSize(getWorldTransform(), region_.get(), getSize());
    shader->popColor();
}
#endif

void Sprite::updateFlip()
{
    if(!region_)
        return;
    uv = region_->getUV();
    updateFlip__();
}

// TL  TR
// BL  BR
// flipX 0-1 2-3
// flipY 0-2 1-3
// flipX & flipY 0-3 1-2
void Sprite::updateFlip__()
{
    auto v=0;
    v |= flipX_ ? 1 : 0;
    v |= flipY_ ? 2 : 0;
    switch (v) {
    case 1: // flipX
        std::swap(uv.uv[0], uv.uv[1]);
        std::swap(uv.uv[2], uv.uv[3]);
        break;
    case 2: // flipY
        std::swap(uv.uv[0], uv.uv[2]);
        std::swap(uv.uv[1], uv.uv[3]);
        break;
    case 3: // flipX & flipY
        std::swap(uv.uv[0], uv.uv[3]);
        std::swap(uv.uv[1], uv.uv[2]);
        break;
    case 4:
    default:
        break;
    }
}
