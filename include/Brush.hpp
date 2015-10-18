#ifndef BRUSH_HPP
#define BRUSH_HPP
#include "Node.hpp"
#include "PrimitiveShader.hpp"

class Brush
{
public:
    Brush();
};
class Renderer;
class RectShape : public Node
{
public:
    RectShape();
    virtual ~RectShape();
    virtual void DrawSelf(Renderer *)override;
};

#endif // BRUSH_HPP
