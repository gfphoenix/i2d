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
    auto r = region_.get();
    if(!r)
        return;
    auto shader = static_cast<SpriteShader*>(getShader());
    renderer->Use(shader);
    shader->pushColor(getColor());
    shader->drawSize(getWorldTransform(), r, getSize());
    shader->popColor();
}

Sprite::~Sprite(){}
Sprite::Sprite(const Ref_ptr<TextureRegion2D> &region) : BaseSprite(region)
  , flipX_(false)
  , flipY_(false)
{
    auto r = region.get();
    if(r)
        uv = r->getUV();
}
Sprite::Sprite() :BaseSprite()
    , flipX_(false)
    , flipY_(false)
{}

void Sprite::setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize)
{
    BaseSprite::setTextureRegion(region, resetSize);
    updateFlip();
}

void Sprite::DrawSelf(Renderer *renderer)
{
    auto r = region_.get();
    if(!r)
        return;
    auto shader = static_cast<SpriteShader*>(getShader());
    renderer->Use(shader);
    shader->pushColor(getColor());
    shader->drawUV(getWorldTransform(), r->getTexture2D().get(), uv, getSize());
    shader->popColor();
}

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
