#ifndef PEN_HPP
#define PEN_HPP
#include <types.hpp>

class PrimitiveShader;
class Pen
{
public:
    Pen(PrimitiveShader *shader);
    inline void setPosition(float x, float y){x_=x;y_=y;}
    inline void setPosition(const Vec2 &pos){setPosition(pos.x,pos.y);}
    inline float getX()const{return x_;}
    inline float getY()const{return y_;}
    inline void setColor(const Color3 &c3){setColor(Color4(c3,1));}
    inline void setColor(const Color4 &c){color_=c;}
    inline const Color4 getColor()const{return color_;}

//    void drawRect(float x, float y, float width, float height);
//    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
//    void drawLine(float x1, float y1, float x2, float y2);

    // curve

protected:
    PrimitiveShader *shader_;
    Color4 color_;
    float x_;
    float y_;
};

#endif // PEN_HPP
