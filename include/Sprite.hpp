#ifndef _SPRITE_HPP
#define _SPRITE_HPP

#include "types.hpp"
#include "Node.hpp"

#include <Texture.hpp>
#include <VertexUVColor.hpp>

class BaseSprite : public Node
{
    public:
        BaseSprite():Node(){}
        virtual const Texture2D *getTexture()=0;
        virtual size_t getObjectNumber()=0;
	virtual const V224_Quad &getDataAt(int index)=0;
};

// set default shader
class Sprite : public BaseSprite
{
    protected:
        V224_Quad quad_;
        Ref_ptr<TextureRegion2D> region_;
        bool flipX_;
        bool flipY_;
        virtual void fillQuad();
        virtual void fillPosition();
        virtual void fillUV();
        virtual void fillColor();
        void setFlipX__();
        void setFlipY__();
        virtual ~Sprite();
    public:
        Sprite();
        virtual const Texture2D *getTexture()override{return region_ ? region_->getTexture2D().get():nullptr;}
        virtual size_t getObjectNumber()override{return 1;}
    virtual const V224_Quad &getDataAt(int )override{return quad_;}
        //inline void init(const Ref_ptr<TextureRegion2D> &region){setTextureRegion(region);}
        virtual void DrawSelf(Renderer *scene)override;
        virtual void onChangedColor()override;
        virtual void onChangedTransformation()override;
        virtual void setTextureRegion(const Ref_ptr<TextureRegion2D> &region, bool resetSize=true);
        // flip
        inline void setFlipX(){flipX_=!flipX_;setFlipX__();}
        inline void setFlipY(){flipY_=!flipY_;setFlipY__();}
        inline bool isFlippedX()const{return flipX_;}
        inline bool isFlippedY()const{return flipY_;}
};

#endif
