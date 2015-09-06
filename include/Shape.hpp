#ifndef SHAPE_HPP
#define SHAPE_HPP
#include "types.hpp"
#include "Node.hpp"
#include "PrimitiveShader.hpp"

class Shape : public Node
{
public:
    inline Shape():Node(){shader_=PrimitiveShader::getInstance();}
};
class Line : public Shape
{
protected:
    friend class MM<Line>;
    Vec2 p0_;
    Vec2 p1_;
public:
    inline Line():Shape(){}
    void setP0(const Vec2 &p0){p0_=p0;}
    void setP1(const Vec2 &p1){p1_=p1;}
    virtual void DrawSelf(Renderer *renderer)override;
};

#endif // SHAPE_HPP

