#include "Brush.hpp"
#include "Renderer.hpp"

Brush::Brush()
{

}

RectShape::RectShape()
{
    setShader(PrimitiveShader_PC::getInstance());
}
RectShape::~RectShape(){}

//RectShape::RectShape(float x, float y, float width, float height)
//{
    //setShader(PrimitiveShader::getInstance());
    //setPosition(x,y);
    //setSize(width, height);
//}

void RectShape::DrawSelf(Renderer *r)
{
    auto sh = static_cast<PrimitiveShader_PC*>(getShader());
    r->Use(sh);
    auto const &an = getAnchor();
    auto const &pos = getPosition();
    auto const &sz = getSize();
    Vec2 arrs[4];
    arrs[0] = Vec2(pos.x - sz.x*an.x, pos.y - sz.y*an.y);
    arrs[1] = Vec2(arrs[0].x+sz.x, arrs[0].y);
    arrs[2] = Vec2(arrs[0].x, arrs[0].y+sz.y);
    arrs[3] = Vec2(arrs[0].x+sz.x, arrs[0].y+sz.y);
    sh->pushColor(getColor());
    sh->drawTriangleStrip(getWorldTransform(), &arrs[0], 4);
    sh->popColor();
}
