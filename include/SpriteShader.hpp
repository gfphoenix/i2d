#ifndef _DEFAULT_SHADER_HPP
#define _DEFAULT_SHADER_HPP
#include <Ref.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <vector>
#include <VertexUVColor.hpp>

class Renderer;
class SpriteShader : public Shader
{
    protected:
        // attribute location
        GLint model_xy_;
        GLint uv_;
        GLint color_;
        // uniform variable
        GLint pv_;
        GLint texSampler_;
        // buffer id
        GLuint vbo_;
        GLuint elementBuffer_;
        int max_;
        const Texture2D *lastTexture_;

        // vertex of a rectangle, BL-BR-TL,TR
        std::vector<V2F_T2F_C4F> buffer_;
        std::vector<unsigned short> indices_;
        std::vector<Color4> colors_;
    protected:
        friend class Shader;
        void init(GLuint program);
        void initElementIndex(int nRect);
        void prepare(const Texture2D *texture);
    public:
        void pushColor(const Color4 &c){colors_.push_back(c);}
        void replaceColor(const Color4 &c){colors_.back()=c;}
        void popColor(){colors_.pop_back();}
        const Color4 &getColor4()const{return colors_.back();}

        virtual void draw(const TextureRegion2D *region);
        virtual void drawOffset(const TextureRegion2D *region, const Vec2 &blXY);
        virtual void drawSize(const TextureRegion2D *region, const Vec2 &size);
        virtual void draw(const TextureRegion2D *region, const Vec2 &blXY, const Vec2 &size);

        virtual void draw(const Mat3 &transform, const TextureRegion2D *region);
        virtual void drawOffset(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &blXY);
        virtual void drawSize(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &size);
        virtual void draw(const Mat3 &transform, const TextureRegion2D *region, const Vec2 &blXY, const Vec2 &size);

        virtual void drawBatch(const Texture2D *region, const V2F_T2F_C4F *data, int n);

        virtual void drawUV(const Texture2D *texture, const UV &uv, const Vec2 &size);
        virtual void drawUV(const Mat3 &transform, const Texture2D *texture, const UV &uv, const Vec2 &size);

        virtual void Render(Node *node)override;
        virtual void Flush()override;
        SpriteShader();
        virtual ~SpriteShader();
        void pushData(const Vec2 &xy, const Vec2 &uv, const Color4 &color);
};
Shader *getDefaultShader();
#endif
