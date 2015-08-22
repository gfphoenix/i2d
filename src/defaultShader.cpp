#include <defaultShader.hpp>
#include <Sprite.hpp>
#include <Camera.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>
#include <Shader.hpp>
#include <mm.hpp>
#include <gl>
#include <vector>
static Shader *shader_=nullptr;
void dShader::init2()
{
    model_xy_ = Shader::getAttribLocation("vertexPosition_model");
    uv_ = Shader::getAttribLocation("vertexUV");
    color_ = Shader::getAttribLocation("vertexColor");
    texSampler_ = Shader::getUniformLocation("mySampler2D");
    pv_ = getUniformLocation("myPV");
    glGenBuffers(1, &vbo_);
    printf("dShader::init2 vbo = %d\n", vbo_);
}
//Shader *dShader::shader_=nullptr;
Shader *Shader::getDefaultShader()
{
    if(shader_ != nullptr)
        return shader_;
    //auto id = Shader::loadStrings(vsrc, fsrc);
    //auto id = Shader::loadFiles("defVertShader.shader", "defFragShader.shader");
    auto id = Shader::loadFiles("V2F_T2F_C4F_vert.es2.0", "V2F_T2F_C4F_frag.es2.0");
    if(id==0)
        return nullptr;
    auto sh = MM<dShader>::New();
    sh->init(id);
    shader_ = sh;
    return sh;
}
dShader::dShader(){}
void dShader::init(GLuint program)
{
    setProgramId(program);
    min_ = 8192;
    max_ = 16384;

    init2();
    buffer_.reserve(min_);
    Debug("model=%d, uv=%d, color=%d, texSampler=%d, pv=%d, vbo=%d\n",
            model_xy_, uv_, color_, texSampler_, pv_, vbo_);
}
dShader::~dShader()
{
    model_xy_ = Shader::getAttribLocation("model_xy");
    uv_ = Shader::getAttribLocation("uv");
    color_ = Shader::getAttribLocation("color");
    glDeleteBuffers(1, &vbo_);
    vbo_ = -1;
    printf("dShader::~dShader \n");
    //Shader::~Shader();
}
//void dShader::LimitBatchNumber(int min, int max)
//{
//    min_ = min;
//    max_ = max;
//    buffer_.reserve(min);
//}

void dShader::Use(Renderer *renderer)
{
    use();
}
class BaseSprite;
void dShader::Render(Renderer *renderer, Node*node)
{
    //Debug("dShader Render ...\n");
    Assert(renderer!=nullptr && node!=nullptr, "Shader::Render : scene or node is nil");
   // auto bs = dynamic_cast<BaseSprite*>(node);
    auto bs = reinterpret_cast<BaseSprite*>(node);
    Assert(bs!=nullptr, "Default shader to render an unknown Node");
    auto tex = bs->getTexture();
    Assert(tex!=nullptr, "BaseSprite has a nil texture");
    if(lastTexture_ != tex){
        if(lastTexture_ != nullptr)
            Flush(renderer);
        lastTexture_ = tex;
    }
    if(buffer_.size()>=max_){
        Flush(renderer);
    }

    auto n = bs->getObjectNumber();
    {
        for(auto i=0; i<n; i++){
            auto &data = bs->getDataAt(i);
            buffer_.push_back(data.bl);
            buffer_.push_back(data.br);
            buffer_.push_back(data.tl);

            buffer_.push_back(data.tl);
            buffer_.push_back(data.br);
            buffer_.push_back(data.tr);
            
//            for(auto j=0; j<4; j++){
//                auto &tmp = data.vtc[j];
//            printf("%s - [%d]  xy=(%f, %f), uv(%f, %f), color=(%f,%f,%f,%f)\n",
//                    bs->getName().c_str(), j, tmp.v.x, tmp.v.y,
//                    tmp.t.s, tmp.t.t,
//                    tmp.c.r, tmp.c.g, tmp.c.b, tmp.c.a
//                    );
//            }
        }
    }
}
void dShader::Flush(Renderer *renderer)
{
    if(lastTexture_==nullptr || buffer_.size()==0)
        return;
    Use(renderer);
    CheckGL();

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    auto const &pv = renderer->getCamera().getPV();
    glUniformMatrix4fv(pv_, 1, GL_FALSE, &pv[0][0]);
    CheckGL();
    if(false){
        auto &m = pv;
        printf("\n");
        for(int u=0; u<4; u++){
            for(int v=0; v<4; v++){
                printf("%f  ", m[u][v]);
            }
            printf("\n");
        }
        auto cc=Vec4(480, 800,0,1);
        cc = m * cc;
        printf("center xy=(%f, %f, %f, %f)\n", cc.x, cc.y, cc.z, cc.w);

    }
    if(false){
        auto id = lastTexture_->getId();
        auto ok = glIsTexture(id) == GL_TRUE;
        if(!ok){
            printf("Bad texture object\n");
        }
        ok = glIsBuffer(vbo_) == GL_TRUE;
        if(!ok){
            printf("Bad buffer %d = \n", vbo_);
        }

    }

    glEnable(GL_TEXTURE_2D);
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    lastTexture_->bind();
    CheckGL();
    glUniform1i(texSampler_, 0);
    CheckGL();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_T2F_C4F)*buffer_.size(), (const void*)&buffer_[0], GL_DYNAMIC_DRAW);
    //printf("Size = %d\n", sizeof(V2F_T2F_C4F)*buffer_.size());
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

    glDrawArrays(GL_TRIANGLES, 0, buffer_.size());
    //CheckGL();

    glDisableVertexAttribArray(model_xy_);
    glDisableVertexAttribArray(uv_);
    glDisableVertexAttribArray(color_);
    glDisable(GL_BLEND);
    Unuse();
    buffer_.clear();
    renderer->addRenderCall();
}

