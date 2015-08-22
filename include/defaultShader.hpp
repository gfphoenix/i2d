#ifndef _DEFAULT_SHADER_HPP
#define _DEFAULT_SHADER_HPP
#include <Ref.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <vector>
#include <VertexUVColor.hpp>

class Renderer;
class dShader : public Shader
{
    private:
        // attribute location
        GLint model_xy_;
        GLint uv_;
        GLint color_;

        GLint pv_;
        GLint texSampler_;
        GLuint vbo_;
	int min_, max_;
        const Texture2D *lastTexture_;
        std::vector<V2F_T2F_C4F> buffer_;
    public:
	void init2();
//	virtual void LimitBatchNumber(int min, int max)override;
    virtual void Use(Renderer *renderer)override;
    virtual void Render(Renderer *renderer, Node *node)override;
    virtual void Flush(Renderer *renderer)override;
	dShader();
	void init(GLuint program);
        virtual ~dShader();
        void pushData(const Vec2 &xy, const Vec2 &uv, const Color4 &color);
};
Shader *getDefaultShader();
#endif
