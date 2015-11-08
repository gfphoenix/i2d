#ifndef PRIMITIVESHADER_HPP
#define PRIMITIVESHADER_HPP
#include "Shader.hpp"
#include "types.hpp"
#include <stack>
#include <vector>
class Node;
class PrimitiveShader : public Shader
{
public:
    static PrimitiveShader *getInstance();
    enum class DrawMode
    {
        POINTS = GL_POINTS,
        LINES   = GL_LINES,
        TRIANGLES   = GL_TRIANGLES,
    };
    void pushTransform(const Mat3 &M);
    void replaceTransform(const Mat3 &M);
    void pushMulTransform(const Mat3 &M);
    void popTransform();
    inline const Mat3 &getTransform()const{return transforms_.top();}
    inline bool hasTransform()const{return !transforms_.empty();}

    // points
    virtual void drawPoint(const Vec2 &point)=0;
    virtual void drawPoints(const Vec2 *points, size_t N)=0;
    //virtual void drawPoint(const iVec2 &point)=0;
    //virtual void drawPoints(const iVec2 *points, size_t N)=0;

    // lines
    virtual void drawLine(const Vec2 &p0, const Vec2 &p1)=0;
    virtual void drawLines(const Vec2 *points, size_t N)=0;
    virtual void drawLineStrip(const Vec2 *points, size_t N)=0;
    virtual void drawLineLoop(const Vec2 *points, size_t N)=0;
    //virtual void drawLine(const iVec2 &p0, const iVec2 &p1);
    //virtual void drawLines(const iVec2 *points, size_t N);
    //virtual void drawLineStrip(const iVec2 *points, size_t N);
    //virtual void drawLineLoop(const iVec2 *points, size_t N);

    // triangles
    virtual void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)=0;
    virtual void drawTriangles(const Vec2 *points, size_t N)=0;
    virtual void drawTriangleStrip(const Vec2 *points, size_t n)=0;
    virtual void drawTriangleFan(const Vec2 *points, size_t n)=0;
    //virtual void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)=0;
    //virtual void drawTriangles(const Vec2 *points, size_t N)=0;
    //virtual void drawTriangleStrip(const Vec2 *points, size_t n)=0;
    //virtual void drawTriangleFan(const Vec2 *points, size_t n)=0;

// ############################################################################
    // multiply transform
    // points
    virtual void drawPoint(const Mat3 &transform, const Vec2 &point)=0;
    virtual void drawPoints(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    //virtual void drawPoint(const Mat3 &transform, const iVec2 &point)=0;
    //virtual void drawPoints(const Mat3 &transform, const iVec2 *points, size_t N)=0;

    // lines
    virtual void drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1)=0;
    virtual void drawLines(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    virtual void drawLineStrip(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    virtual void drawLineLoop(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    //virtual void drawLine(const Mat3 &transform, const iVec2 &p0, const iVec2 &p1);
    //virtual void drawLines(const Mat3 &transform, const iVec2 *points, size_t N);
    //virtual void drawLineStrip(const Mat3 &transform, const iVec2 *points, size_t N);
    //virtual void drawLineLoop(const Mat3 &transform, const iVec2 *points, size_t N);

    // triangles
    virtual void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)=0;
    virtual void drawTriangles(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    virtual void drawTriangleStrip(const Mat3 &transform, const Vec2 *points, size_t n)=0;
    virtual void drawTriangleFan(const Mat3 &transform, const Vec2 *points, size_t n)=0;
    //virtual void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)=0;
    //virtual void drawTriangles(const Mat3 &transform, const Vec2 *points, size_t N)=0;
    //virtual void drawTriangleStrip(const Mat3 &transform, const Vec2 *points, size_t n)=0;
    //virtual void drawTriangleFan(const Mat3 &transform, const Vec2 *points, size_t n)=0;
// ##############################################################################


    // shutcut
    inline void drawPoint(float x, float y)
    {   drawPoint(Vec2(x,y));    }
    inline void drawPoint(const Mat3 &transform, float x, float y)
    {   drawPoint(transform, Vec2(x,y)); }
    //inline void drawPoint(const Mat3 &transform, const iVec2 &p){drawPoint(transform, p.x, p.y);}

    inline void drawLine(float x0, float y0, float x1, float y1)
    {   drawLine(Vec2(x0,y0), Vec2(x1,y1)); }
    inline void drawLine(const Mat3 &transform, float x0, float y0, float x1, float y1)
    {   drawLine(transform, Vec2(x0,y0), Vec2(x1,y1));   }

protected:
    std::stack<Mat3> transforms_;
    // uniform variable
    GLint pv_;

    DrawMode drawMode_;
};

class PrimitiveShader_PC : public PrimitiveShader
{
protected:
    PrimitiveShader_PC();
    void init();
public:
    struct Point {
        Vec2 pos;
        Vec4 color;
    };
    inline virtual ~PrimitiveShader_PC(){}
    static PrimitiveShader_PC *getInstance();
    void pushColor(const Color4 &c){colors_.push(c);}
    void replaceColor(const Color4 &c){colors_.top()=c;}
    void popColor(){colors_.pop();}
    const Color4 &getColor4()const{return colors_.top();}


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
    virtual void drawBatch(DrawMode mode, const Point *p, size_t n);
    virtual void drawBatch(DrawMode mode, const Vec2 *p, size_t n);

    virtual void drawPoint(const Vec2 &p){drawPoint(p,getColor4());}
    virtual void drawPoints(const Vec2 *points, size_t N);
    virtual void drawPoint(const Vec2 &p, const Color4 &color);

    virtual void drawLine(const Vec2 &p0, const Vec2 &p1);
    virtual void drawLines(const Vec2 *points, size_t N);
    virtual void drawLineStrip(const Vec2 *points, size_t n);
    virtual void drawLineStrip(const Point *points, size_t n);
    virtual void drawLineLoop(const Vec2 *points, size_t n);
    virtual void drawLineLoop(const Point *points, size_t n);

    inline void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2){drawTriangle(p0,p1,p2,getColor4());}
    inline void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &color)
    {
        drawTriangle(p0,p1,p2,color,color,color);
    }
    virtual void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2);
    virtual void drawTriangles(const Vec2 *points, size_t n);
    virtual void drawTriangleStrip(const Vec2 *points, size_t n);
    virtual void drawTriangleStrip(const Point *points, size_t n);
    virtual void drawTriangleFan(const Vec2 *points, size_t n);
    virtual void drawTriangleFan(const Point *points, size_t n);

    virtual void drawRect(const Vec2 &bl, const Vec2 &size);
    virtual void drawRect(float x, float y, float w, float h);
// transform
    virtual void drawBatch(const Mat3 &transform, DrawMode mode, const Point *p, size_t N);
    virtual void drawBatch(const Mat3 &transform, DrawMode mode, const Vec2 *p, size_t N);

    inline void drawPoint(const Mat3 &transform, const Vec2 &p){drawPoint(transform,p,getColor4());}
    virtual void drawPoints(const Mat3 &transforms, const Vec2 *points, size_t N);
    virtual void drawPoint(const Mat3 &transform, const Vec2 &p, const Color4 &color);

    virtual void drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1);
    virtual void drawLines(const Mat3 &transform, const Vec2 *points, size_t N);
    virtual void drawLineStrip(const Mat3 &transform, const Vec2 *points, size_t n);
    virtual void drawLineStrip(const Mat3 &transform, const Point *points, size_t n);
    virtual void drawLineLoop(const Mat3 &transform, const Vec2 *points, size_t n);
    virtual void drawLineLoop(const Mat3 &transform, const Point *points, size_t n);

    inline void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2){drawTriangle(transform,p0,p1,p2,getColor4());}
    inline void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &color)
    {
        drawTriangle(transform,p0,p1,p2,color,color,color);
    }
    virtual void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2);
    virtual void drawTriangles(const Mat3 &transforms, const Vec2 *points, size_t n);
    virtual void drawTriangleStrip(const Mat3 &transform, const Vec2 *points, size_t n);
    virtual void drawTriangleStrip(const Mat3 &transform, const Point *points, size_t n);
    virtual void drawTriangleFan(const Mat3 &transform, const Vec2 *points, size_t n);
    virtual void drawTriangleFan(const Mat3 &transform, const Point *points, size_t n);
protected:
    inline void pushPoints(const Vec2 *points, size_t n){pushPoints(points,n,getColor4());}
    void pushPoints(const Vec2 *points, size_t n, const Color4 &color);
    void pushPoints(const Point *points, size_t n);
    inline void pushPoints(const Mat3 &transform, const Vec2 *points, size_t n){pushPoints(transform,points,n,getColor4());}
    void pushPoints(const Mat3 &transform, const Vec2 *points, size_t n, const Color4 &color);
    void pushPoints(const Mat3 &transform, const Point *points, size_t n);

protected:
    friend class MM<PrimitiveShader_PC>;
    static Ref_ptr<PrimitiveShader_PC> self;
    std::vector<Point> buffer_; // real data for draw
    std::stack<Color4> colors_;

    // attrib
    GLint model_xy_;
    GLint color_;
    // buffer id, for attribute
    GLuint vbo_;
};

#endif // PRIMITIVESHADER_HPP
