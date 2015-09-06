#include "Shape.hpp"
#include "Renderer.hpp"

void Line::DrawSelf(Renderer *renderer)
{
    auto sh = static_cast<PrimitiveShader*>(getShader());
    renderer->Use(sh);

    sh->pushColor(getColor());
    sh->pushTransform(getWorldTransform());

    Vec2 p[3] = {p0_, p1_, Vec2(200,600)};
    sh->drawLineLoop(p, 3);
//    sh->drawLineLoop(p0_, p1_);

    sh->popTransform();
    sh->popColor();

}
