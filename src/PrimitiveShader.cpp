#include "PrimitiveShader.hpp"
#include "Renderer.hpp"
#include "mm.hpp"

/**
static const char *vsrc = "\n"
"void main(){\n"
"}\n";
*/
PrimitiveShader::PrimitiveShader():
    vbo_(0)
{
    colors_.push(Color4(1,1,1,1));
}
void PrimitiveShader::init()
{
    model_xy_ = getAttribLocation("i2d_Vertex");
    color_  = getAttribLocation("i2d_Color");
    pv_ = getUniformLocation("i2d_PV");
    glGenBuffers(1, &vbo_);
}

PrimitiveShader *PrimitiveShader::getInstance()
{
    if(!self){
        auto id = Shader::loadFiles("geom_vertex.txt", "geom_frag.txt");
        if(id<=0)
            return nullptr;
        auto sh = MM<PrimitiveShader>::New();
        sh->setProgramId(id);
        sh->init();
        self = sh;
    }
    return self.get();
}
void PrimitiveShader::pushTransform(const Mat3 &M)
{
    Flush();
    transforms_.push(M);
}
void PrimitiveShader::replaceTransform(const Mat3 &M)
{
    Flush();
    transforms_.top() = M;
}

void PrimitiveShader::pushMulTransform(const Mat3 &M)
{
    Flush();
    auto const &top = transforms_.top();
    transforms_.push(top * M);
}
void PrimitiveShader::popTransform()
{
    Flush();
    transforms_.pop();
}
Ref_ptr<PrimitiveShader> PrimitiveShader::self = nullptr;
// no transform
void PrimitiveShader::drawPoint(const Vec2 &p, const Color4 &color)
{
    prepare(DrawMode::POINTS);
    buffer_.push_back(Point{p,color});
}

void PrimitiveShader::drawLine(const Vec2 &p0, const Vec2 &p1)
{
    prepare(DrawMode::LINES);
    buffer_.push_back(Point{p0, getColor4()});
    buffer_.push_back(Point{p1, getColor4()});
}
void PrimitiveShader::drawLineStrip(const Vec2 *points, int n)
{
    prepare(DrawMode::LINES);
    int N = (int)buffer_.size();
    int lines = (n-1);
    buffer_.resize(N+2*lines);
    auto p = &buffer_[N];
    auto const & color = getColor4();
    for(int i=0; i<lines; i++){
        p->pos = *points;
        p->color = color;
        p++;
        p->pos = *++points;
        p->color = color;
        p++;
    }
}
void PrimitiveShader::drawLineStrip(const Point *points, int n)
{
    prepare(DrawMode::LINES);
    int N = (int)buffer_.size();
    int lines = (n-1);
    buffer_.resize(N+2*lines);
    auto p = &buffer_[N];
    for(int i=0; i<lines; i++){
        *p++ = *points;
        *p++ = *++points;
    }
}
void PrimitiveShader::drawLineLoop(const Vec2 *points, int n)
{
    drawLineStrip(points, n);
    int N = buffer_.size();
    buffer_.resize(N+2);
    auto p = &buffer_[N];
    p[0].pos = points[n-1];
    p[1].pos = points[0];
    p[0].color = p[1].color = getColor4();
}
void PrimitiveShader::drawLineLoop(const Point *points, int n)
{
    drawLineStrip(points, n);
    buffer_.push_back(points[n-1]);
    buffer_.push_back(points[0]);
}


void PrimitiveShader::drawTriangle(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2)
{
    prepare(DrawMode::TRIANGLES);
    int n = buffer_.size();
    buffer_.resize(n+3);
    auto p = &buffer_[n];
    p->pos = p0; p->color = c0; p++;
    p->pos = p1; p->color = c1; p++;
    p->pos = p2; p->color = c2;
}
void PrimitiveShader::drawTriangleStrip(const Vec2 *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const &color = getColor4();
    auto half = triangles>>1;
    for(int i=0; i<half; i++){
        p->pos = points[0]; p->color = color; p++;
        p->pos = points[1]; p->color = color; p++;
        p->pos = points[2]; p->color = color; p++;
        points++;

        p->pos = points[1]; p->color = color; p++;
        p->pos = points[0]; p->color = color; p++;
        p->pos = points[2]; p->color = color; p++;
        points++;
    }
    if(half*2 != triangles){
        p->pos = points[0]; p->color = color; p++;
        p->pos = points[1]; p->color = color; p++;
        p->pos = points[2]; p->color = color;
    }
}
void PrimitiveShader::drawTriangleStrip(const Point *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto half = triangles>>1;
    for(int i=0; i<half; i++){
        *p++ = points[0];
        *p++ = points[1];
        *p++ = points[2];
        points++;
        *p++ = points[1];
        *p++ = points[0];
        *p++ = points[2];
        points++;
    }
    if(half*2 != triangles){
        *p++ = points[0];
        *p++ = points[1];
        *p++ = points[2];
    }
}
void PrimitiveShader::drawTriangleFan(const Vec2 *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const *q = points+1;
    auto const &color = getColor4();
    while(triangles-- > 0){
        p->pos = *points; p->color = color; p++;
        p->pos = *q++; p->color = color; p++;
        p->pos = *q++; p->color = color; p++;
    }
}
void PrimitiveShader::drawTriangleFan(const Point *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const *q = points+1;
    while(triangles-- > 0){
        *p++ = *points;
        *p++ = *q++;
        *p++ = *q++;
    }
}

void PrimitiveShader::drawBatch(DrawMode mode, const Vec2 *pos, int N)
{
    prepare(mode);
    pushPoints(pos, N);
}
void PrimitiveShader::drawBatch(DrawMode mode, const Point *p, int N)
{
    prepare(mode);
    pushPoints(p, N);
}
void PrimitiveShader::pushPoints(const Vec2 *points, int n, const Color4 &color)
{
    int N = (int)buffer_.size();
    buffer_.resize(N+n);
    auto p = &buffer_[N];
    for(int i=0; i<n; i++){
        p->pos = *points++;
        p->color = color;
        p++;
    }
}
void PrimitiveShader::pushPoints(const Point *points, int n)
{
    buffer_.insert(buffer_.end(), points, points+n);
}


// transform
inline static void transform_helper(Vec2 &out, const Mat3 &transform, const Vec2 &in)
{
    auto v = transform * Vec3(in,1);
    out = Vec2(v);
}
inline static Vec2 transform_helper(const Mat3 &transform, const Vec2 &in)
{
    auto v = transform * Vec3(in,1);
    return Vec2(v);
}

void PrimitiveShader::drawPoint(const Mat3 &transform, const Vec2 &p, const Color4 &color)
{
    prepare(DrawMode::POINTS);
    auto q = transform * Vec3(p,1);
    buffer_.push_back(Point{Vec2(q),color});
}

void PrimitiveShader::drawLine(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1)
{
    prepare(DrawMode::LINES);
    auto q = transform * Vec3(p0,1);
    buffer_.push_back(Point{Vec2(q), getColor4()});
    q = transform * Vec3(p1,1);
    buffer_.push_back(Point{Vec2(q), getColor4()});
}
void PrimitiveShader::drawLineStrip(const Mat3 &transform, const Vec2 *points, int n)
{
    prepare(DrawMode::LINES);
    int N = (int)buffer_.size();
    int lines = (n-1);
    buffer_.resize(N+2*lines);
    auto p = &buffer_[N];
    auto const & color = getColor4();

    for(int i=0; i<lines; i++){
        auto t = transform * Vec3(*points, 1);
        p->pos = Vec2(t);
        p->color = color;
        p++;
        t = transform * Vec3(*++points, 1);
        p->pos = Vec2(t);
        p->color = color;
        p++;
    }
}
void PrimitiveShader::drawLineStrip(const Mat3 &transform, const Point *points, int n)
{
    prepare(DrawMode::LINES);
    int N = (int)buffer_.size();
    int lines = (n-1);
    buffer_.resize(N+2*lines);
    auto p = &buffer_[N];
    for(int i=0; i<lines; i++){
        auto t = transform * Vec3(points->pos, 1);
        *p++ = Point{Vec2(t),points->color};
        ++points;
        t = transform * Vec3(points->pos, 1);
        *p++ = Point{Vec2(t),points->color};
    }
}
void PrimitiveShader::drawLineLoop(const Mat3 &transform, const Vec2 *points, int n)
{
    drawLineStrip(transform, points, n);
    buffer_.push_back(Point{transform_helper(transform, points[n-1]),getColor4()});
    buffer_.push_back(Point{transform_helper(transform, *points),getColor4()});
}
void PrimitiveShader::drawLineLoop(const Mat3 &transform, const Point *points, int n)
{
    drawLineStrip(transform, points, n);
    buffer_.push_back(Point{transform_helper(transform, points[n-1].pos),points[n-1].color});
    buffer_.push_back(Point{transform_helper(transform, points[0].pos),points->color});
}

void PrimitiveShader::drawTriangle(const Mat3 &transform, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Color4 &c0, const Color4 &c1, const Color4 &c2)
{
    prepare(DrawMode::TRIANGLES);
    auto t = transform * Vec3(p0,1);
    buffer_.push_back(Point{Vec2(t), c0});
    t = transform * Vec3(p1,1);
    buffer_.push_back(Point{Vec2(t), c1});
    t = transform * Vec3(p2,1);
    buffer_.push_back(Point{Vec2(t), c2});
}
void PrimitiveShader::drawTriangleStrip(const Mat3 &transform, const Vec2 *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const &color = getColor4();
    auto half = triangles>>1;
    for(int i=0; i<half; i++){
        *p++ = Point{transform_helper(transform, points[0]), color};
        *p++ = Point{transform_helper(transform, points[1]), color};
        *p++ = Point{transform_helper(transform, points[2]), color};
        points++;
        *p++ = Point{transform_helper(transform, points[1]), color};
        *p++ = Point{transform_helper(transform, points[0]), color};
        *p++ = Point{transform_helper(transform, points[2]), color};
        points++;
    }
    if(half*2 != triangles){
       *p++ = Point{transform_helper(transform, points[0]), color};
       *p++ = Point{transform_helper(transform, points[1]), color};
       *p++ = Point{transform_helper(transform, points[2]), color};
    }
}
void PrimitiveShader::drawTriangleStrip(const Mat3 &transform, const Point *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto half = triangles>>1;
    for(int i=0; i<half; i++){
        *p++ = Point{transform_helper(transform, points[0].pos), points[0].color};
        *p++ = Point{transform_helper(transform, points[1].pos), points[0].color};
        *p++ = Point{transform_helper(transform, points[2].pos), points[0].color};
       points++;
       *p++ = Point{transform_helper(transform, points[1].pos), points[0].color};
       *p++ = Point{transform_helper(transform, points[0].pos), points[0].color};
       *p++ = Point{transform_helper(transform, points[2].pos), points[0].color};
      points++;
    }
    if(half*2 != triangles){
          *p++ = Point{transform_helper(transform, points[0].pos), points[0].color};
          *p++ = Point{transform_helper(transform, points[1].pos), points[0].color};
          *p++ = Point{transform_helper(transform, points[2].pos), points[0].color};
    }
}
void PrimitiveShader::drawTriangleFan(const Mat3 &transform, const Vec2 *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const *q = points+1;
    auto const &color = getColor4();
    auto p0 = Point{transform_helper(transform, *points), color};
    while(triangles-- > 0){
        *p++ = p0;
        *p++ = Point{transform_helper(transform, *q++), color};
        *p++ = Point{transform_helper(transform, *q++), color};
    }
}
void PrimitiveShader::drawTriangleFan(const Mat3 &transform, const Point *points, int n)
{
    prepare(DrawMode::TRIANGLES);
    int triangles = n-2;
    int N = buffer_.size();
    buffer_.resize(N+3*triangles);
    auto p = &buffer_[N];
    auto const *q = points+1;
    auto p0 = Point{transform_helper(transform,points->pos), points->color};
    while(triangles-- > 0){
        *p++ = p0;
        *p++ = Point{transform_helper(transform, q->pos), q->color};q++;
        *p++ = Point{transform_helper(transform, q->pos), q->color};q++;
    }
}

void PrimitiveShader::drawBatch(const Mat3 &transform, DrawMode mode, const Point *p, int N)
{
    prepare(mode);
    pushPoints(transform, p, N);
}
void PrimitiveShader::drawBatch(const Mat3 &transform, DrawMode mode, const Vec2 *p, int N)
{
    prepare(mode);
    pushPoints(transform, p, N);
}
void PrimitiveShader::pushPoints(const Mat3 &transform, const Vec2 *points, int n, const Color4 &color)
{
    int N = (int)buffer_.size();
    buffer_.resize(N+n);
    auto p = &buffer_[N];
    for(int i=0; i<n; i++){
        *p++ = Point{transform_helper(transform, *points++), color};
    }
}
void PrimitiveShader::pushPoints(const Mat3 &transform, const Point *points, int n)
{
    int N = (int)buffer_.size();
    buffer_.resize(N+n);
    auto p = &buffer_[N];
    for(int i=0; i<n; i++){
        *p++ = Point{transform_helper(transform, points->pos), points->color};
        points++;
    }
}

// end of draw
void PrimitiveShader::Render(Node *node)
{

}

void PrimitiveShader::Flush()
{
    if(buffer_.empty())
        return;
    auto renderer = getRenderer();
    Use();
    CheckGL();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    auto const &pv = renderer->getCamera().getPV();
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
    CheckGL();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point)*buffer_.size(), (const void*)&buffer_[0], GL_DYNAMIC_DRAW);
    CheckGL();

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(model_xy_);
    glVertexAttribPointer(
            model_xy_,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            sizeof(Point),      // stride
            (void*)0            // array buffer offset
            );
    CheckGL();

    // 2nd attribute buffer : color
    glEnableVertexAttribArray(color_);
    glVertexAttribPointer(
            color_,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Point),
            (void*)(2*sizeof(GLfloat))
            );
    CheckGL();
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays((GLenum)drawMode_, 0, buffer_.size());
    glDisable(GL_PROGRAM_POINT_SIZE);
    CheckGL();

    glDisableVertexAttribArray(model_xy_);
    glDisableVertexAttribArray(color_);

    Unuse();
    buffer_.clear();
    renderer->addRenderCall();
}
