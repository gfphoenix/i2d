#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include "types.hpp"
#include <memory>
#include <Buffer.hpp>
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

enum class RegionDirection;
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

// region direction, the bottom direction
enum class RegionDirection
{
    BOTTOM = 0,
    RIGHT,
    LEFT,
    TOP,
};
#include <gl>
enum class Filter
{
};
enum class Wrap
{
};
// when texture2d is created, it(std::shared_ptr<Texture2D>) is owned by assetmanager.
// then users can hold or release the resources safely
class Texture2D final: public Resource
{
    private:
        GLuint id_;
        int width_;
        int height_;
        // GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA,
        int format_;
        void Dispose()override;
        void init(const Image_RGBA8 &image);
    protected:
        Texture2D():Resource(),self(nullptr){}
        // should only be accessed by texture-manager
        static Texture2D *create(ResourceManager *manager, const std::string &name);
        static Texture2D *create(ResourceManager *manager, const Image_RGBA8 &image, const std::string &name);
        static void deleteSelf(TextureRegion2D *r, void *selfT2d);
        TextureRegion2D *self;
        friend class TextureManager;
    DECLARE_PLACEMENT
    public:
        const std::string getInfo()const override{return std::string("texture2d");}
        inline int getWidth()const{return width_;}
        inline int getHeight()const{return height_;}
        inline int   getFormat()const{return format_;}
        inline GLuint  getId()const{return id_;}

        // origin point is at top left
        Ref_ptr<TextureRegion2D> getTextureRegion(int offx, int offy, int width, int height, RegionDirection dir);
        Ref_ptr<TextureRegion2D> getTextureRegion(RegionDirection dir=RegionDirection::BOTTOM);
        inline void bind()const{::glBindTexture(GL_TEXTURE_2D, id_);}
        inline void unbind()const{::glBindTexture(GL_TEXTURE_2D,0);}
};
// Only can be created by TextureRegion2D::getSubRegion() or Texture2D::getTextureRegion(), or
// indirectly created by TextureAtlas::getTextureRegion(), which uses Texture2d::getTextureRegion
// internally.
// Note: only to be resource if given it a name and set a manager by texture-atlas
class TextureRegion2D final : public Ref
{
    private:
        Ref_ptr<Texture2D> texture_;
        void (*onDestroy)(TextureRegion2D *self, void *data);
        void *destroyData_;
        // descripted by tl_ and size_.
        // tl_ must non-negative, and size_ must not be zero, negative means rotation
        iVec2 tl_;
        iVec2 size_;
        Vec2 uv_tl_;
        Vec2 uv_br_;
        TextureRegion2D():Ref()
        {
            onDestroy=nullptr;
            destroyData_=nullptr;
        }

        //DEL_COPY(TextureRegion2D);
        friend class Texture2D;
        friend class TextureAtlas;

	template <typename _tp>
	friend class MM;

protected:
    virtual void Delete()override;
    virtual void setDeleteCallback(void (*fn)(TextureRegion2D *self,void*data), void*data);
    void init(Texture2D *tex, const iVec2 &tl, const iVec2 &size, RegionDirection dir);
    public:
        // when remove, call ondestroy if set
    inline bool isRotated()const{return size_.x<0 || size_.y<0;}
        virtual const std::string getInfo()const{
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "texture-region mother=%p ", texture_.get());
            return std::string(tmp);
        }
        TextureRegion2D(TextureRegion2D &&region);
        inline const Ref_ptr<Texture2D> &getTexture2D()const{return texture_;}
        inline const Vec2 &getUVtl()const{return uv_tl_;}
        inline const Vec2 &getUVbr()const{return uv_br_;}
        UV getUV()const;
        void getUV(Vec2 &bl, Vec2 &br, Vec2 &tl, Vec2 &tr)const;
        UVi getUVi()const;
        inline int getRegionWidth()const{return ((size_.x<0)^(size_.y<0))?std::abs(size_.y):std::abs(size_.x);}
        inline int getRegionHeight()const{return ((size_.x<0)^(size_.y<0))?std::abs(size_.x):std::abs(size_.y);}
        inline iVec2 getRegionSize()const{
            auto w=std::abs(size_.x);
            auto h=std::abs(size_.y);
            return (size_.x<0)^(size_.y<0) ? iVec2(h,w) : iVec2(w,h);
        }
        inline iVec2 getRectSize()const{return iVec2(std::abs(size_.x),std::abs(size_.y));}
        RegionDirection getRegionDirection()const;
        Ref_ptr<TextureRegion2D> getSubRegion(int offx, int offy, int width, int height, RegionDirection dir)const;
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
struct AtlasConfig final
{
    std::vector<AtlasConfig> config_;
    AtlasConfig(AtlasConfig &&cfg):
        config_(std::move(cfg.config_)){}
    AtlasConfig(){}
};
typedef bool (*loadTA_func_t)(AtlasConfig &config, const Buffer &buffer);
// texture-atlas is created and hold by texture-manager
// texture-region getting from texture-atlas is rotated properly
class TextureAtlas final : public Resource
{
    private:
        static std::vector<std::pair<std::string,loadTA_func_t>> loaders_;
        std::unordered_map<std::string, Ref_ptr<Texture2D>> textures_;
        std::unordered_map<std::string, Ref_ptr<TextureRegion2D>> regions_;
        inline void Dispose(){}
        friend class TextureManager;
    protected:
        static AtlasConfig parseAtlasConfig(const std::string &atlasName);
        static AtlasConfig parseAtlasConfig(const std::string &atlasName, const Buffer &buffer);
        static TextureAtlas *create(const AtlasConfig &config);
        static TextureAtlas *create(const std::string &atlasName);
        const std::string getInfo()const{return std::string("texture-atlas");}
    public:
        Ref_ptr<TextureRegion2D> get(const std::string &name)const;
        static void registerLoader(const char *suffix, loadTA_func_t fn);
};
#endif
