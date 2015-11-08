#ifndef PRIMITIVESHADER_PUC
#define PRIMITIVESHADER_PUC
#include "types.hpp"
#include "PrimitiveShader.hpp"
#include <vector>
// geometry shader :
// position + color(uniform)
class PrimitiveShader_PuC : public PrimitiveShader
{
    public:
    static PrimitiveShader_PuC *create(const char *vSrc, const char *fSrc);
    static PrimitiveShader_PuC *createFromFile(const char *vFilePath, const char *fFilePath);
    // points
    virtual void drawPoint(const Vec2 &point)override;
    virtual void drawPoints(const Vec2 *points, size_t N)override;

    // lines
    virtual void drawLine(const Vec2 &p0, const Vec2 &p1)override;
    virtual void drawLines(const Vec2 *points, size_t N)override;
    virtual void drawLineStrip(const Vec2 *points, size_t N)override;
    virtual void drawLineLoop(const Vec2 *points, size_t N)override;

    // triangles
    virtual void drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)override;
    virtual void drawTriangles(const Vec2 *points, size_t N)override;
    virtual void drawTriangleStrip(const Vec2 *points, size_t n)override;
    virtual void drawTriangleFan(const Vec2 *points, size_t n)override;

// ############################################################################
    // multiply transform
    // points
    virtual void drawPoint(const Mat3 &transform, const Vec2 &point)override;
    virtual void drawPoints(const Mat3 &transform, const Vec2 *points, size_t N)override;

    // lines
    virtual void drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1)override;
    virtual void drawLines(const Mat3 &transform, const Vec2 *points, size_t N)override;
    virtual void drawLineStrip(const Mat3 &transform, const Vec2 *points, size_t N)override;
    virtual void drawLineLoop(const Mat3 &transform, const Vec2 *points, size_t N)override;

    // triangles
    virtual void drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)override;
    virtual void drawTriangles(const Mat3 &transform, const Vec2 *points, size_t N)override;
    virtual void drawTriangleStrip(const Mat3 &transform, const Vec2 *points, size_t n)override;
    virtual void drawTriangleFan(const Mat3 &transform, const Vec2 *points, size_t n)override;
// ##############################################################################

    virtual void Flush()override;
protected:
    PrimitiveShader_PuC();
    virtual ~PrimitiveShader_PuC();
    bool init();
//    void setBufferSize(size_t x6);
    inline void prepare(DrawMode mode)
    {
        if(drawMode_!=mode){
            if(!buffer_.empty())
                Flush();
            drawMode_ = mode;
        }
    }
    void drawLinesOrTriangles(const Vec2 *points, size_t N);
    inline void pushPoints(const Vec2 *points, size_t N)
    {
        auto i=buffer_.size();
        buffer_.insert(buffer_.end(), points, points+N);
        for(auto end=i+N; i<end; i++)
            indices_.push_back(i);
    }

    // add N indices
    inline void pushLines(const Vec2 *points, size_t N)
    {
        pushPoints(points, N);
    }
    // add 2*(N-1) indices
    inline void pushLineStrip(const Vec2 *points, size_t N)
    {
        auto i=buffer_.size();
        buffer_.insert(buffer_.end(), points, points+N);
        for(auto end=i+N-1; i<end; i++){
            indices_.push_back(i);
            indices_.push_back(i+1);
        }
    }
    // add 2*N indices
    inline void pushLineLoop(const Vec2 *points, size_t N)
    {
        auto i=buffer_.size();
        buffer_.insert(buffer_.end(), points, points+N);
        for(auto end=i+N-1; i<end; i++){
            indices_.push_back(i);
            indices_.push_back(i+1);
        }
        indices_.push_back(i+N-1);
        indices_.push_back(i);
    }
    // N must be x3
    inline void pushTriangles(const Vec2 *points, size_t N)
    {
        pushPoints(points, N);
    }
// N must be >=3
    inline void pushTriangleStrip(const Vec2 *points, size_t N)
    {
        auto i = buffer_.size();
        buffer_.insert(buffer_.end(), points, points+N);
        auto r = N & 1;
        N   -= r;
        for(auto end=i+N-2; i<end;){
            indices_.push_back(i);
            indices_.push_back(i+1);
            indices_.push_back(i+2);
            i++;
            indices_.push_back(i+1);
            indices_.push_back(i);
            indices_.push_back(i+2);
            i++;
        }
        if(r){
            indices_.push_back(i);
            indices_.push_back(i+1);
            indices_.push_back(i+2);
        }
    }
    // p0, P0,P1; P1,P2
    // produce N-1 triangles
    inline void pushTriangleFan(const Vec2 &p0, const Vec2 *points, size_t N)
    {

        auto i = buffer_.size();
        buffer_.push_back(p0);
        buffer_.insert(buffer_.end(), points, points+N);
        for(auto j=i+1, end=i+N; j<end;){
            indices_.push_back(i);
            indices_.push_back(j);
            indices_.push_back(++j);
        }
    }

    GLint pv_;
    GLint colorID_; // uniform : color
    Color4 color_;  // used to store color uniform value

    GLint model_xy_;    // vertex data - attribute
    GLuint vbo_;    // gl buffer object for vertex
    GLuint ebo_;    // gl buffer object for vertex index (ARRAY_ELEMENT)

    std::vector<Vec2> buffer_;
    std::vector<uint16_t> indices_; // max size = 65532.
};

#endif // PRIMITIVESHADER_PUC

