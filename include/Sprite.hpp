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

class Sprite : public BaseSprite
{
    protected:
        UV uv; // to support flip
        bool flipX_;
        bool flipY_;

        void updateFlip__();
        void updateFlip();
        virtual ~Sprite();
    public:
        Sprite();
        Sprite(const Ref_ptr<TextureRegion2D> &region);
        virtual void DrawSelf(Renderer *renderer)override;
        virtual void setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize=true)override;

        // flip
        inline void setFlipX(bool flipX){flipX_=flipX;updateFlip();}
        inline void setFlipY(bool flipY){flipY_=flipY;updateFlip();}
        inline void toggleFlipX(){flipX_=!flipX_;updateFlip();}
        inline void toggleFlipY(){flipY_=!flipY_;updateFlip();}
        inline bool isFlippedX()const{return flipX_;}
        inline bool isFlippedY()const{return flipY_;}
};

#endif
