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
Sprite::Sprite(const Ref_ptr<TextureRegion2D> &region) : BaseSprite(region)
  , flipX_(false)
  , flipY_(false)
{}
Sprite::Sprite() :BaseSprite()
    , flipX_(false)
    , flipY_(false)
{}

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
void Sprite::setFlipX__()
{}
void Sprite::setFlipY__()
{}
