#include <Renderer.hpp>
#include <Shader.hpp>

// must be called in DrawSelf
void Renderer::Use(Shader *shader)
{
    if(shader == nullptr || shader == last_)
        return ;
    if(last_ != nullptr)
        last_->Flush();
    last_ = shader;
    shader->setRenderer(this);
}
// only used in stage
void Renderer::Flush()
{
    if(last_ != nullptr){
        last_->Flush();
        last_->setRenderer(nullptr);
    }
    last_ = nullptr;
}
