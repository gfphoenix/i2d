#include <SpriteShader.hpp>
#include <Sprite.hpp>
#include <Camera.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>
#include <Shader.hpp>
#include <mm.hpp>
#include <gl>
#include <vector>
#define GLSL(src) "\n" #src
static const GLchar* vSrc = GLSL(
attribute vec2 vertexPosition_model;
attribute vec2 vertexUV;
attribute vec4 vertexColor;
varying vec2 myUV;
varying vec4 myColor;
uniform mat4 myPV;
void main(){
gl_Position =  myPV * vec4(vertexPosition_model, 0, 1);
myUV = vertexUV;
myColor = vertexColor;
}
);

static const GLchar *fSrc = GLSL(
#ifdef GL_ES
precision lowp float;
#endif
varying vec2 myUV;
varying vec4 myColor;
uniform sampler2D mySampler2D;
void main(){
vec4 tmp = myColor * texture2D(mySampler2D, myUV);
gl_FragColor = clamp(tmp, 0.0, 1.0);
}
);
Ref_ptr<SpriteShader> SpriteShader::self=nullptr;
SpriteShader *SpriteShader::getInstance()
{
    if(!self){
        //auto id = Shader::loadStrings(vsrc, fsrc);
        //auto id = Shader::loadFiles("defVertShader.shader", "defFragShader.shader");
        //auto id = Shader::loadFiles("V2F_T2F_C4F_vert.es2.0", "V2F_T2F_C4F_frag.es2.0");
        auto id = Shader::loadStrings(vSrc, fSrc);
        if(id==0)
            return nullptr;
        auto sh = MM<SpriteShader>::New();
        sh->init(id);
        self = sh;
    }
    return self.get();
}

SpriteShader::SpriteShader():
    Shader()
    , vbo_(0)
    , elementBuffer_(0)
    , lastTexture_(nullptr)
{colors_.push(Color4(1,1,1,1));}
// number of N rectangles
void SpriteShader::initElementIndex(int N)
{
    auto n = 6*N;
    auto &v = indices_;
    v.resize(n);
    auto d = static_cast<unsigned short*>(v.data());
    unsigned short index = 0;
    for(int i=0; i<N; i++){
        *d++ = index;
        *d++ = index+1;
        *d++ = index+2;
        *d++ = index+2;
        *d++ = index+1;
        *d++ = index+3;
        index += 4;
    }
    if(elementBuffer_==0)
        glGenBuffers(1, &elementBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*indices_.size(), &indices_[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SpriteShader::init(GLuint program)
{
    setProgramId(program);
    max_ = 4*10922; // max number of vertex: (65535/6*4)

    model_xy_ = Shader::getAttribLocation("vertexPosition_model");
    uv_ = Shader::getAttribLocation("vertexUV");
    color_ = Shader::getAttribLocation("vertexColor");

    texSampler_ = Shader::getUniformLocation("mySampler2D");
    pv_ = getUniformLocation("myPV");
    glGenBuffers(1, &vbo_);
    buffer_.reserve(4*8192);
    initElementIndex(max_/4);
}
SpriteShader::~SpriteShader()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &elementBuffer_);
    vbo_ = 0;
    elementBuffer_=0;
}
void SpriteShader::pushTransform(const Mat3 &M)
{
    Flush();
    transforms_.push(M);
}
void SpriteShader::replaceTransform(const Mat3 &M)
{
    Flush();
    transforms_.top() = M;
}

void SpriteShader::pushMulTransform(const Mat3 &M)
{
    Flush();
    auto const &top = transforms_.top();
    transforms_.push(top * M);
}
void SpriteShader::popTransform()
{
    Flush();
    transforms_.pop();
}

void SpriteShader::prepare(const Texture2D *texture)
{
    if(lastTexture_ != texture){
        if(lastTexture_ != nullptr)
            Flush();
        lastTexture_ = texture;
    }else if((int)buffer_.size()>=max_){
        Flush();
    }
}

void SpriteShader::draw(const TextureRegion2D *region)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto sz = region->getRegionSize();
    p0.v = Vec2(0,0);
    p1.v = Vec2(sz.x, 0);
    p2.v = Vec2(0, sz.y);
    p3.v = Vec2(sz.x, sz.y);
}

void SpriteShader::drawOffset(const TextureRegion2D *region, const Vec2 &blXY)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto sz = region->getRegionSize();
    p0.v = blXY;
    p1.v = Vec2(blXY.x + sz.x, blXY.y);
    p2.v = Vec2(blXY.x, blXY.y+sz.y);
    p3.v = Vec2(blXY.x+sz.x, blXY.y+sz.y);
}

void SpriteShader::drawSize(const TextureRegion2D *region, const Vec2 &size)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    p0.v = Vec2(0,0);
    p1.v = Vec2(size.x, 0);
    p2.v = Vec2(0, size.y);
    p3.v = Vec2(size.x, size.y);
}

void SpriteShader::draw(const TextureRegion2D *region, const Vec2 &blXY, const Vec2 &size)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    p0.v = blXY;
    p1.v = Vec2(blXY.x + size.x, blXY.y);
    p2.v = Vec2(blXY.x, blXY.y+size.y);
    p3.v = Vec2(blXY.x+size.x, blXY.y+size.y);
}
void SpriteShader::drawBatch(const Texture2D *texture, const V2F_T2F_C4F *data, int n)
{
    prepare(texture);
    n = n>0 ? ((n>>2)<<2) : n;
    if(n<=0)
        return;
    auto Nindex = max_;
    while(n>0){
        auto m = Nindex - (int)buffer_.size();
        if(n<m)
            m = n;
        buffer_.insert(buffer_.end(), data, data+m);
        data += m;
        n -= m;
        if(n>0) // still left
            Flush();
    }
}

void SpriteShader::draw(const Mat3 &transform, const TextureRegion2D *region)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    auto const &cc = getColor4();
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto sz = region->getRegionSize();
    auto aw = transform[0][0] * sz.x;
    auto bw = transform[0][1] * sz.x;
    auto ch = transform[1][0] * sz.y;
    auto dh = transform[1][1] * sz.y;
    p0.v = Vec2(transform[2]);
    p1.v = Vec2(aw + transform[2][0], bw + transform[2][1]);
    p2.v = Vec2(ch + transform[2][0], dh + transform[2][1]);
    p3.v = Vec2(aw+ch+transform[2][0], ch+dh+transform[2][1]);
}

void SpriteShader::drawOffset(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &blXY)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto sz = region->getRegionSize();
    auto ps = transform * Mat4x3(
            Vec3(blXY,1), Vec3(blXY.x+sz.x, blXY.y, 1),
            Vec3(blXY.x, blXY.y+sz.y, 1),
            Vec3(blXY.x+sz.x, blXY.y+sz.y, 1));
    p0.v.x = ps[0].x;
    p0.v.y = ps[0].y;
    p1.v.x = ps[1].x;
    p1.v.y = ps[1].y;
    p2.v.x = ps[2].x;
    p2.v.y = ps[2].y;
    p3.v.x = ps[3].x;
    p3.v.y = ps[3].y;
}

void SpriteShader::drawSize(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &size)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto const & sz = size;
    auto aw = transform[0][0] * sz.x;
    auto bw = transform[0][1] * sz.x;
    auto ch = transform[1][0] * sz.y;
    auto dh = transform[1][1] * sz.y;
    p0.v = Vec2(transform[2]);
    p1.v = Vec2(aw + transform[2][0], bw + transform[2][1]);
    p2.v = Vec2(ch + transform[2][0], dh + transform[2][1]);
    p3.v = Vec2(aw+ch+transform[2][0], bw+dh+transform[2][1]);
}

void SpriteShader::draw(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &blXY, const Vec2 &size)
{
    prepare(region->getTexture2D().get());
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    region->getUV(p0.t, p1.t, p2.t, p3.t);
    p0.c = p1.c = p2.c = p3.c = cc;

    auto const &sz = size;
    auto ps = transform * Mat4x3(
            Vec3(blXY,1),
            Vec3(blXY.x+sz.x, blXY.y, 1),
            Vec3(blXY.x, blXY.y+sz.y, 1),
            Vec3(blXY.x+sz.x, blXY.y+sz.y, 1));
    p0.v.x = ps[0].x;
    p0.v.y = ps[0].y;
    p1.v.x = ps[1].x;
    p1.v.y = ps[1].y;
    p2.v.x = ps[2].x;
    p2.v.y = ps[2].y;
    p3.v.x = ps[3].x;
    p3.v.y = ps[3].y;
}
void SpriteShader::drawUV(const Texture2D *texture, const UV &uv, const Vec2 &size)
{
    prepare(texture);
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    p0.t = uv.uv[0];
    p1.t = uv.uv[1];
    p2.t = uv.uv[2];
    p3.t = uv.uv[3];
    p0.c = p1.c = p2.c = p3.c = cc;

    p0.v = Vec2(0,0);
    p1.v = Vec2(size.x, 0);
    p2.v = Vec2(0, size.y);
    p3.v = Vec2(size.x, size.y);
}

void SpriteShader::drawUV(const Mat3 &transform, const Texture2D *texture, const UV &uv, const Vec2 &size)
{
    prepare(texture);
    auto n = buffer_.size();
    buffer_.resize(n+4);
    auto const &cc = getColor4();
    auto &p0 = buffer_[n];
    auto &p1 = buffer_[n+1];
    auto &p2 = buffer_[n+2];
    auto &p3 = buffer_[n+3];
    p0.t = uv.uv[0];
    p1.t = uv.uv[1];
    p2.t = uv.uv[2];
    p3.t = uv.uv[3];
    p0.c = p1.c = p2.c = p3.c = cc;

    auto const & sz = size;
    auto aw = transform[0][0] * sz.x;
    auto bw = transform[0][1] * sz.x;
    auto ch = transform[1][0] * sz.y;
    auto dh = transform[1][1] * sz.y;
    p0.v = Vec2(transform[2]);
    p1.v = Vec2(aw + transform[2][0], bw + transform[2][1]);
    p2.v = Vec2(ch + transform[2][0], dh + transform[2][1]);
    p3.v = Vec2(aw+ch+transform[2][0], bw+dh+transform[2][1]);
}

class BaseSprite;
void SpriteShader::Render(Node*node)
{
    Assert(node!=nullptr, "Shader::Render : node is nil");
    auto bs = dynamic_cast<BaseSprite*>(node);
    Assert(bs!=nullptr, "Default shader to render an unknown Node");
    auto region = bs->getTextureRegion();
    pushColor(node->getColor());
    drawSize(node->getWorldTransform(), region, node->getSize());
    popColor();
}
void SpriteShader::Flush()
{
    if(lastTexture_==nullptr || buffer_.size()==0)
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

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    lastTexture_->bind();
    CheckGL();
    glUniform1i(texSampler_, 0);
    CheckGL();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_T2F_C4F)*buffer_.size(), (const void*)&buffer_[0], GL_DYNAMIC_DRAW);
    CheckGL();

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(model_xy_);
    glVertexAttribPointer(
            model_xy_,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            sizeof(V2F_T2F_C4F),                  // stride
            (void*)0            // array buffer offset
            );
    CheckGL();

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(uv_);
    glVertexAttribPointer(
            uv_,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            sizeof(V2F_T2F_C4F),                                // stride
            (void*)(2*sizeof(GLfloat))                          // array buffer offset
            );
    CheckGL();
    glEnableVertexAttribArray(color_);
    glVertexAttribPointer(
            color_,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(V2F_T2F_C4F),
            (void*)(4*sizeof(GLfloat))
            );
    CheckGL();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer_);
    glDrawElements(GL_TRIANGLES, buffer_.size()/4*6, GL_UNSIGNED_SHORT, (void*)0);

    CheckGL();

    glDisableVertexAttribArray(model_xy_);
    glDisableVertexAttribArray(uv_);
    glDisableVertexAttribArray(color_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_BLEND);
    Unuse();
    buffer_.clear();
    renderer->addRenderCall();
}

