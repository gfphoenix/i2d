#include <Renderer.hpp>
#include <Scene.hpp>
#include <Sprite.hpp>
#include <defaultShader.hpp>
#include <string.h>
Sprite::~Sprite()
{
}
Sprite::Sprite()
    :BaseSprite()
     , flipX_(false)
     , flipY_(false)
{
    auto sh = Shader::getDefaultShader();
    setShader(sh);
    fillColor();
}

void Sprite::DrawSelf(Renderer *renderer)
{
    if(!region_)
        return;
    auto shader = getShader();
    fillQuad();
    renderer->Use(shader);
    shader->Render(renderer, this);
}
void Sprite::onChangedTransformation()
{
    fillPosition();
}
void Sprite::onChangedColor()
{
    fillColor();
}
void Sprite::fillQuad()
{
    fillPosition();
    fillUV();
    fillColor();
}
// or Mat4x3
void Sprite::fillPosition()
{
	auto const &cwt = getWorldTransform();
    auto const &size = getSize();
	Vec3 result;
	result = cwt * Vec3(0,0,1);
    ::memcpy(&quad_.bl.v, &result[0], sizeof(quad_.bl.v));
    result = cwt * Vec3(size.x, 0, 1);
    ::memcpy(&quad_.br.v, &result[0], sizeof(quad_.br.v));
    result = cwt * Vec3(0, size.y, 1);
    ::memcpy(&quad_.tl.v, &result[0], sizeof(quad_.tl.v));
    result = cwt * Vec3(size,1);
    ::memcpy(&quad_.tr.v, &result[0], sizeof(quad_.tr.v));
   // Debug("Position=(%.2f, %.2f)--(%.2f, %.2f) -- (%.2f, %.2f) - (%.2f, %.2f)\n",
   //       quad_.bl.v.x, quad_.bl.v.y,
   //       quad_.br.v.x, quad_.br.v.y,
   //       quad_.tl.v.x, quad_.tl.v.y,
   //       quad_.tr.v.x, quad_.tr.v.y
   //       );
}
void Sprite::fillUV()
{
    auto &&t = region_->getUV();
    quad_.bl.t = t.uv[0];
    quad_.br.t = t.uv[1];
    quad_.tl.t = t.uv[2];
    quad_.tr.t = t.uv[3];
}
void Sprite::fillColor()
{
    ::memcpy(&quad_.bl.c, &color_, sizeof(Color4));
    ::memcpy(&quad_.br.c, &color_, sizeof(Color4));
    ::memcpy(&quad_.tl.c, &color_, sizeof(Color4));
    ::memcpy(&quad_.tr.c, &color_, sizeof(Color4));
}

void Sprite::setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize)
{
    region_=region;
    if(!region){
        setSize(0,0);
        return ;
    }
    if(resetSize)
        setSize(region_->getSize());
    fillUV();
    if(flipX_)
        setFlipX();
    if(flipY_)
        setFlipY();
}
void Sprite::setFlipX__()
{
    flipX_ = !flipX_;
    std::swap(quad_.bl.t, quad_.tl.t);
    std::swap(quad_.br.t, quad_.tr.t);
}
void Sprite::setFlipY__()
{
    flipY_ = !flipY_;
    std::swap(quad_.bl.t, quad_.br.t);
    std::swap(quad_.tl.t, quad_.tr.t);
}
