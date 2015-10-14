#ifndef PRIMITIVESHADER_HPP
#define PRIMITIVESHADER_HPP
#include "Shader.hpp"
#include "types.hpp"
#include <stack>
#include <vector>
class Node;

class PrimitiveShader : public Shader
{
protected:
    PrimitiveShader();
    void init();
public:
    enum class DrawMode
    {
        POINTS = GL_POINTS,
        LINES   = GL_LINES,
        TRIANGLES   = GL_TRIANGLES,
    };
    struct Point {
        Vec2 pos;
        Vec4 color;
    };
    inline virtual ~PrimitiveShader(){}
    static PrimitiveShader *getInstance();
    void pushColor(const Color4 &c){colors_.push(c);}
    void replaceColor(const Color4 &c){colors_.top()=c;}
    void popColor(){colors_.pop();}
    const Color4 &getColor4()const{return colors_.top();}
    void pushTransform(const Mat3 &M);
    void replaceTransform(const Mat3 &M);
    void pushMulTransform(const Mat3 &M);
    void popTransform();
    inline const Mat3 &getTransform()const{return transforms_.top();}
    inline bool hasTransform()const{return !transforms_.empty();}

    inline void prepare(DrawMode mode)
    {
        if(drawMode_!=mode){
            if(!buffer_.empty())
                Flush();
            drawMode_ = mode;
        }
    }
    virtual void Render(Node *node);
    virtual void Flush();
// no transform
    virtual void drawBatch(DrawMode mode, const Point *p, int n);
    virtual void drawBatch(DrawMode mode, const Vec2 *p, int n);

    inline void drawPoint(const Vec2 &p){drawPoint(p,getColor4());}
    virtual void drawPoint(const Vec2 &p, const Color4 &color);

    virtual void drawLine(const Vec2 &p0, const Vec2 &p1);
    virtual void drawLineStrip(const Vec2 *points, int n);
    virtual void drawLineStrip(const Point *points, int n);
    virtual void drawLineLoop(const Vec2 *points, int n);
    virtual void drawLineLoop(const Point *points, int n);

    inline void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2){drawTriangle(p0,p1,p2,getColor4());}
    inline void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &color)
    {
        drawTriangle(p0,p1,p2,color,color,color);
    }
    virtual void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2);
    virtual void drawTriangleStrip(const Vec2 *points, int n);
    virtual void drawTriangleStrip(const Point *points, int n);
    virtual void drawTriangleFan(const Vec2 *points, int n);
    virtual void drawTriangleFan(const Point *points, int n);

// transform
    virtual void drawBatch(const Mat3 &transform, DrawMode mode, const Point *p, int N);
    virtual void drawBatch(const Mat3 &transform, DrawMode mode, const Vec2 *p, int N);

    inline void drawPoint(const Mat3 &transform, const Vec2 &p){drawPoint(transform,p,getColor4());}
    virtual void drawPoint(const Mat3 &transform, const Vec2 &p, const Color4 &color);

    virtual void drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1);
    virtual void drawLineStrip(const Mat3 &transform, const Vec2 *points, int n);
    virtual void drawLineStrip(const Mat3 &transform, const Point *points, int n);
    virtual void drawLineLoop(const Mat3 &transform, const Vec2 *points, int n);
    virtual void drawLineLoop(const Mat3 &transform, const Point *points, int n);

    inline void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2){drawTriangle(transform,p0,p1,p2,getColor4());}
    inline void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &color)
    {
        drawTriangle(transform,p0,p1,p2,color,color,color);
    }
    virtual void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2);
    virtual void drawTriangleStrip(const Mat3 &transform, const Vec2 *points, int n);
    virtual void drawTriangleStrip(const Mat3 &transform, const Point *points, int n);
    virtual void drawTriangleFan(const Mat3 &transform, const Vec2 *points, int n);
    virtual void drawTriangleFan(const Mat3 &transform, const Point *points, int n);
protected:
    inline void pushPoints(const Vec2 *points, int n){pushPoints(points,n,getColor4());}
    void pushPoints(const Vec2 *points, int n, const Color4 &color);
    void pushPoints(const Point *points, int n);
    inline void pushPoints(const Mat3 &transform, const Vec2 *points, int n){pushPoints(transform,points,n,getColor4());}
    void pushPoints(const Mat3 &transform, const Vec2 *points, int n, const Color4 &color);
    void pushPoints(const Mat3 &transform, const Point *points, int n);

protected:
    friend class MM<PrimitiveShader>;
    static Ref_ptr<PrimitiveShader> self;
    std::vector<Point> buffer_; // real data for draw
    std::stack<Color4> colors_;
    std::stack<Mat3> transforms_;
    // attrib
    GLint model_xy_;
    GLint color_;
    // uniform variable
    GLint pv_;
    // buffer id
    GLuint vbo_;
    DrawMode drawMode_;
};

#endif // PRIMITIVESHADER_HPP
