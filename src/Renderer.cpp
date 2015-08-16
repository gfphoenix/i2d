#include <Renderer.hpp>
#include <Shader.hpp>

class StageLayer;
void Renderer::Use(Shader *shader, StageLayer *stage)
{
    if(shader == nullptr || shader == last_)
        return ;
    if(last_ != shader && last_ != nullptr)
        last_->Flush(this, stage);
    last_ = shader;
}
void Renderer::Flush(StageLayer *stage)
{
    if(last_ != nullptr)
        last_->Flush(this, stage);
    last_ = nullptr;
}
