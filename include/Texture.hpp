#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include "types.hpp"
#include <memory>
#include <Ref.hpp>
#include <ResourceManager.hpp>
#include <TextureManager.hpp>
#include <TextureLoader.hpp>
#include <unordered_map>
#include <vector>
#include <stdio.h>

class Texture2D;
class TextureRegion2D;
class TextureAtlas;

// region direction, the bottom direction
enum class RegionDirection
{
    BOTTOM = 0,
    RIGHT,
    LEFT,
    TOP,
    INVALID,
};
// 4 point in texture, and its order is:
// [0] -> BL, [1] -> BR, [2] -> TL, [3] -> TR
struct UV
{
    Vec2 uv[4];
};
struct UVi
{
    iVec2 uv[4];
};

// Only can be created by TextureRegion2D::getSubRegion() or Texture2D::getTextureRegion(), or
// indirectly created by TextureAtlas::getTextureRegion(), which uses Texture2d::getTextureRegion
// internally.
// Note: only to be resource if given it a name and set a manager by texture-atlas
class TextureRegion2D final : public Ref
{
    private:
        Ref_ptr<Texture2D> texture_;
        // descripted by tl_ and size_.
        // tl_ must non-negative, and size_ must not be zero, negative means rotation
        iVec2 tl_;
        iVec2 size_;
        TextureRegion2D():Ref(){}
        inline bool isRotated()const{return size_.x>0 && size_.y>0;}
        //DEL_COPY(TextureRegion2D);
        friend class Texture2D;
        friend class TextureAtlas;

	template <typename _tp>
	friend class MM;
    public:
        virtual const std::string getInfo()const{
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "texture-region mother=%p ", texture_.get());
            return std::string(tmp);
        }
        TextureRegion2D(TextureRegion2D &&region);
        inline const Ref_ptr<Texture2D> getTexture2D()const{return texture_;}
        UV getUV()const;
        void getUV(Vec2 &bl, Vec2 &br, Vec2 &tl, Vec2 &tr)const;
        UVi getUVi()const;
        inline int getWidth()const{return ((size_.x<0)^(size_.y<0))?std::abs(size_.y):std::abs(size_.x);}
        inline int getHeight()const{return ((size_.x<0)^(size_.y<0))?std::abs(size_.x):std::abs(size_.y);}
        inline iVec2 getSize()const{
            auto w=std::abs(size_.x);
            auto h=std::abs(size_.y);
            return (size_.x<0)^(size_.y<0) ? iVec2(h,w) : iVec2(w,h);
        }
        RegionDirection getRegionDirection()const;
        Ref_ptr<TextureRegion2D> getSubRegion(int offx, int offy, int width, int height)const;
};
#include <gl>
enum class Filter
{
};
enum class Wrap
{
};
struct Item 
{
    std::string name; // texture region name in .atlas file
    int offx;
    int offy;
    int width;
    int height;
    RegionDirection rotation; // must be -90, 0, 90. it is anti-clock-wise
};
struct TextureConfig 
{
    std::string textureName_; // file name
    Filter filter_;
    Wrap wrap_;
    std::vector<Item> items_;
};
struct AtlasConfig
{
    std::vector<AtlasConfig> config_;
};
// texture-atlas is created and hold by texture-manager
// texture-region getting from texture-atlas is rotated properly
class TextureAtlas final : public Resource
{
    private:
        std::unordered_map<std::string, Ref_ptr<Texture2D>> textures_;
        std::unordered_map<std::string, Ref_ptr<TextureRegion2D>> regions_;
        inline void Dispose(){}
        friend class TextureManager;
    protected:
        static AtlasConfig parseAtlasConfig(const std::string &atlasName);
        static TextureAtlas *create(const AtlasConfig &config);
        static TextureAtlas *create(const std::string &atlasName);
        const std::string getInfo()const{return std::string("texture-atlas");}
    public:
        Ref_ptr<TextureRegion2D> get(const std::string &name)const;
};
#endif
