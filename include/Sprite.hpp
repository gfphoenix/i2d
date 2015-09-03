#ifndef _SPRITE_HPP
#define _SPRITE_HPP

#include "types.hpp"
#include "Node.hpp"

#include <Texture.hpp>
#include <VertexUVColor.hpp>

class BaseSprite : public Node
{
    protected:
        Ref_ptr<TextureRegion2D> region_;
    public:
        BaseSprite(const Ref_ptr<TextureRegion2D> &region);
        inline BaseSprite():Node(){initShader();}
        virtual ~BaseSprite(){}
        const TextureRegion2D *getTextureRegion()const{return region_.get();}
        virtual const Texture2D *getTexture(){return region_ ? region_->getTexture2D().get():nullptr;}
        virtual void setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize=true);
        virtual void DrawSelf(Renderer *renderer)override;
private:
        void initShader();
};
class LightSprite : public BaseSprite
{
protected:
    UV uv;
public:
    LightSprite(const Ref_ptr<TextureRegion2D> &region);
    inline LightSprite():BaseSprite(){}
    virtual ~LightSprite(){}
    virtual void setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize=true)override;
    virtual void DrawSelf(Renderer *renderer)override;
};

class Sprite : public BaseSprite
{
    protected:
        //V224_Quad quad_;
        bool flipX_;
        bool flipY_;

        void setFlipX__();
        void setFlipY__();
        virtual ~Sprite();
    public:
        Sprite();
        Sprite(const Ref_ptr<TextureRegion2D> &region);
        virtual void DrawSelf(Renderer *scene)override;
        // flip
        inline void setFlipX(){flipX_=true;setFlipX__();}
        inline void setFlipY(){flipY_=true;setFlipY__();}
        inline void toggleFlipX(){flipX_=!flipX_;setFlipX__();}
        inline void toggleFlipY(){flipY_=!flipY_;setFlipY__();}
        inline bool isFlippedX()const{return flipX_;}
        inline bool isFlippedY()const{return flipY_;}
};

#endif
