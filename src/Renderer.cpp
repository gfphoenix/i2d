#include <Renderer.hpp>
#include <Shader.hpp>

class StageLayer;
void Renderer::Use(Shader *shader)
{
    if(shader == nullptr || shader == last_)
        return ;
    if(last_ != shader && last_ != nullptr)
        last_->Flush(this);
    last_ = shader;
}
void Renderer::Flush()
{
    if(last_ != nullptr)
        last_->Flush(this);
    last_ = nullptr;
}
