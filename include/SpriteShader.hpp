#ifndef _DEFAULT_SHADER_HPP
#define _DEFAULT_SHADER_HPP
#include "types.hpp"
#include <Ref.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <stack>
#include <vector>
#include <VertexUVColor.hpp>

class Renderer;
class SpriteShader : public Shader
{
    protected:
        GLuint vShader_;
        GLuint fRGBA_;
        GLuint fAlpha_;

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
        std::stack<Color4> colors_;
        std::stack<Mat3> transforms_;
    protected:
        friend class Shader;
        void init(GLuint program);
        void initElementIndex(int nRect);
        void prepare(const Texture2D *texture);
    public:
        void pushColor(const Color4 &c){colors_.push(c);}
        void replaceColor(const Color4 &c){colors_.top()=c;}
        void popColor(){colors_.pop();}
        const Color4 &getColor4()const{return colors_.top();}
        void pushTransform(const Mat3 &M);
        void replaceTransform(const Mat3 &M);
        void pushMulTransform(const Mat3 &M);
        void popTransform();
        inline const Mat3 &getTransform()const{return transforms_.top();}
        inline bool hasTransform()const{return !transforms_.empty();}

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
        enum class Type{
            ALPHA, RGB, RGBA,
        };

        virtual ~SpriteShader();
        static SpriteShader *getInstance();
        static SpriteShader *getShader(Type type);
        static void DestroyAll();
protected:
        static Ref_ptr<SpriteShader> rgba_;
        static Ref_ptr<SpriteShader> rgb_;
        static Ref_ptr<SpriteShader> alpha_;
        void pushData(const Vec2 &xy, const Vec2 &uv, const Color4 &color);
};
//Shader *getDefaultShader();
#endif
