#include <PrimitiveShader_PuC.hpp>
#include <Log.hpp>
#include <mm.hpp>
#include <Renderer.hpp>

// (1<<16)-1 = 65535
#define MAX_BUFFER_SIZE 65535
// ((1<<16)-1)/6 * 6 = 65532 , x6
#define MAX_INDEX_SIZE  65532
#define MAX_LINES_SIZE  MAX_INDEX_SIZE
#define MAX_TRIANGLE_SIZE   MAX_INDEX_SIZE
// points
void PrimitiveShader_PuC::drawPoint(const Vec2 &point)
{
    prepare(DrawMode::POINTS);
    buffer_.push_back(point);
}
void PrimitiveShader_PuC::drawPoints(const Vec2 *points, size_t N)
{
    prepare(DrawMode::POINTS);
    buffer_.insert(buffer_.end(), points, points+N);
}

void PrimitiveShader_PuC::drawLinesOrTriangles(const Vec2 *points, size_t N)
{
    if(indices_.size() == MAX_INDEX_SIZE)
        Flush();
    auto m = MAX_INDEX_SIZE - indices_.size();
    if(N < m)
        goto out;
    {
    pushPoints(points, m);
    Flush();
    points += m;
    N      -= m;
    while(N>=MAX_INDEX_SIZE){
        pushPoints(points, MAX_INDEX_SIZE);
        Flush();
        points += MAX_INDEX_SIZE;
        N   -= MAX_INDEX_SIZE;
    }
    }
out:
    if(N>0)
        pushPoints(points, N);
}

// lines
void PrimitiveShader_PuC::drawLine(const Vec2 &p0, const Vec2 &p1)
{
    prepare(DrawMode::LINES);
    if(indices_.size() == MAX_INDEX_SIZE)
        Flush();
    auto i = buffer_.size();
    buffer_.push_back(p0);
    buffer_.push_back(p1);
    indices_.push_back(i);
    indices_.push_back(i+1);
}

void PrimitiveShader_PuC::drawLines(const Vec2 *points, size_t N)
{
    {// debug N
        if(N&1)
            Log::d("%s N=%u should be 2n", __func__, N);
    }
    prepare(DrawMode::LINES);
    drawLinesOrTriangles(points, N);
}

// N produces 2*(N-1) index
// N must be >=2
void PrimitiveShader_PuC::drawLineStrip(const Vec2 *points, size_t N)
{
    {//debug N
        if(points==nullptr || N<2)
            Log::d("PrimitiveShader_PuC drawLineStrip points=%p, N=%d", points, (int)N);
    }
    prepare(DrawMode::LINES);
    if(indices_.size() == MAX_INDEX_SIZE)
        Flush();
    auto const left  = (MAX_INDEX_SIZE-indices_.size())/2;
    if(N < left+1)
        goto out;
    { //
    pushLineStrip(points, left+1);
    Flush();
    points += left;
    N      -= left;
    auto const CC = MAX_INDEX_SIZE/2+1;
    while(N>=CC){
        pushLineStrip(points, CC);
        Flush();
        points += CC-1;
        N   -= CC-1;
    }
    }
out:
    if(N>1)
        pushLineStrip(points, N);
}

void PrimitiveShader_PuC::drawLineLoop(const Vec2 *points, size_t N)
{
    {//debug N
        if(points==nullptr || N==0)
            Log::d("PrimitiveShader_PuC drawLineStrip points=%p, N=%d", points, (int)N);
    }
    prepare(DrawMode::LINES);
    if(indices_.size()+N+N<MAX_INDEX_SIZE){
        pushLineLoop(points, N);
    }else{
        drawLineStrip(points, N);
        drawLine(points[N-1], points[0]);
    }
}

// triangles
void PrimitiveShader_PuC::drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)
{
    prepare(DrawMode::TRIANGLES);
    if(indices_.size()==MAX_INDEX_SIZE)
        Flush();
    auto i= buffer_.size();
    indices_.push_back(i++);
    indices_.push_back(i++);
    indices_.push_back(i);

    buffer_.push_back(p0);
    buffer_.push_back(p1);
    buffer_.push_back(p2);
}

void PrimitiveShader_PuC::drawTriangles(const Vec2 *points, size_t N)
{
    prepare(DrawMode::TRIANGLES);
    drawLinesOrTriangles(points, N);
}
// N must be >=3
void PrimitiveShader_PuC::drawTriangleStrip(const Vec2 *points, size_t N)
{
    prepare(DrawMode::TRIANGLES);
    if(indices_.size() == MAX_INDEX_SIZE)
        Flush();
    auto left = MAX_INDEX_SIZE - indices_.size();
    if(3*(N-2)<left)
        goto out;
    {
        auto pairs = left/6;
        if(pairs>0){
            auto Triangles = pairs + pairs;
            pushTriangleStrip(points, Triangles+2);
            points  += Triangles;
            N       -= Triangles;
        }
        Flush();
        auto const VN = MAX_INDEX_SIZE/3+2;
        while(N>=VN){
            pushTriangleStrip(points, VN);
            Flush();
            points  += VN-2;
            N       -= VN-2;
        }
    }
out:
    if(N>2)
        pushTriangleStrip(points, N);
}
// N>=3
// N produces 
void PrimitiveShader_PuC::drawTriangleFan(const Vec2 *const points, size_t N)
{
    //no error checking
    prepare(DrawMode::TRIANGLES);
    if(indices_.size() == MAX_INDEX_SIZE)
        Flush();
    const Vec2 *p = points+1;
    auto NN = N-1;
    // triangles
    auto m = (MAX_INDEX_SIZE - indices_.size())/3;
    if(N < m+2)
        goto out;
    {
        pushTriangleFan(*points, p, m+1);
        Flush();
        p   += m;
        NN  -= m;
        while(3*NN>=MAX_INDEX_SIZE+3){
            pushTriangleFan(*points, p, MAX_INDEX_SIZE/3+1);
            Flush();
            p   += MAX_INDEX_SIZE/3;
            NN  -= MAX_INDEX_SIZE/3;
        }
    }
out:
    if(NN>1)
        pushTriangleFan(*points, p, NN);
}

// ############################################################################
// multiply transform
// points
void PrimitiveShader_PuC::drawPoint(const Mat3 &transform, const Vec2 &point)
{

}

void PrimitiveShader_PuC::drawPoints(const Mat3 &transform, const Vec2 *points, size_t N)
{

}

//void drawPoints(const Mat3 &transform, const iVec2 *points, size_t N)

// lines
void PrimitiveShader_PuC::drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1)
{

}

void PrimitiveShader_PuC::drawLines(const Mat3 &transform, const Vec2 *points, size_t N)
{

}

void PrimitiveShader_PuC::drawLineStrip(const Mat3 &transform, const Vec2 *points, size_t N)
{

}

void PrimitiveShader_PuC::drawLineLoop(const Mat3 &transform, const Vec2 *points, size_t N)
{

}

// triangles
void PrimitiveShader_PuC::drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)
{

}

void PrimitiveShader_PuC::drawTriangles(const Mat3 &transform, const Vec2 *points, size_t N)
{

}

void PrimitiveShader_PuC::drawTriangleStrip(const Mat3 &transform, const Vec2 *points, size_t n)
{

}

void PrimitiveShader_PuC::drawTriangleFan(const Mat3 &transform, const Vec2 *points, size_t n)
{

}

// ##############################################################################
PrimitiveShader_PuC::PrimitiveShader_PuC():
    pv_(-1)
  , colorID_(-1)
  , color_(Vec4(1.0,1.0,1.0,1.0))
  , model_xy_(-1)
  , vbo_(0)
  , ebo_(0)
{

}
PrimitiveShader_PuC::~PrimitiveShader_PuC()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
    vbo_  = ebo_  = 0;
}
PrimitiveShader_PuC *PrimitiveShader_PuC::create(const char *vSrc, const char *fSrc)
{
//    return init(loadStrings(vSrc, fSrc));
    return nullptr;
}

PrimitiveShader_PuC *PrimitiveShader_PuC::createFromFile(const char *vFilePath, const char *fFilePath)
{
//    return init(loadFiles(vFilePath, fFilePath));
    return nullptr;
}

bool PrimitiveShader_PuC::init()
{
    pv_         = getUniformLocation("i2d_PV");
    colorID_    = getUniformLocation("i2d_Color");
    model_xy_   = getAttribLocation("i2d_Vertex");
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    auto ok = pv_>=0 && colorID_>=0 && vbo_>0 && ebo_>0;
    return ok;
}

void PrimitiveShader_PuC::Flush()
{
        if(buffer_.empty())
            return;
        auto renderer = getRenderer();
        Use();
        CheckGL();

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        auto const &pv = renderer->getCamera().getPV();
        glEnable(GL_BLEND);
        if(hasTransform()){
            //m[0][2],m[1][2] is always 0
            auto const &m = getTransform();
            auto pv2 = pv * Mat4(Vec4(m[0][0],m[0][1],0,m[0][2]),
                    Vec4(m[1][0],m[1][1],0,m[1][2]),
                    Vec4(0,0,1,0),
                    Vec4(m[2][0],m[2][1],0,m[2][2]));
            glUniformMatrix4fv(pv_, 1, GL_FALSE, &pv2[0][0]);
        }else{
            glUniformMatrix4fv(pv_, 1, GL_FALSE, &pv[0][0]);
        }
        glUniform4f(colorID_, color_.r, color_.g, color_.b, color_.a);
        CheckGL();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2)*buffer_.size(), (const void*)&buffer_[0], GL_DYNAMIC_DRAW);

        CheckGL();

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(model_xy_);
        glVertexAttribPointer(
                model_xy_,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                sizeof(Vec2),      // stride
                (void*)0            // array buffer offset
                );
        CheckGL();

        glEnable(GL_PROGRAM_POINT_SIZE);
        if(drawMode_ == DrawMode::POINTS){
            glDrawArrays((GLenum)drawMode_, 0, buffer_.size());
        }else{
            auto const n = indices_.size();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vec2)*n, &indices_[0], GL_DYNAMIC_DRAW);
            glDrawElements((GLenum)drawMode_, n, GL_UNSIGNED_SHORT, (void*)0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            indices_.clear();
        }
        glDisable(GL_PROGRAM_POINT_SIZE);
        CheckGL();

        glDisableVertexAttribArray(model_xy_);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisable(GL_BLEND);
        Unuse();
        buffer_.clear();
        renderer->addRenderCall();
    }
