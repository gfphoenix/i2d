#ifndef _TEXTURE_MANAGER_HPP
#define _TEXTURE_MANAGER_HPP
#include <gl>
#include <Ref.hpp>
#include <ResourceManager.hpp>
#include <Texture.hpp>
#include <TextureLoader.hpp>
class Texture2D;
class TextureRegion2D;
class TextureAtlas;
enum class RegionDirection;

// texture-manager manages two type of resource:
// 1. texture -- s single picture
// 2. texture-atlas -- an atlas containing one or more textures, and has many small pictures 
//      which can be rendered independently, called texture-region
class TextureManager final : public ResourceManager
{
    private:
        Texture2D *loadTexture__(const std::string &name);
        TextureAtlas *loadTextureAtlas__(const std::string &name);
	static Ref_ptr<TextureManager> tm_;
    public:
	static TextureManager *getInstance();
    Ref_ptr<Texture2D> getTexture(const std::string &name);

//protected:
        Ref_ptr<Resource> loadResource(const std::string &name)override;
        Ref_ptr<Texture2D> loadTexture(const std::string &name);
        Ref_ptr<TextureAtlas> loadTextureAtlas(const std::string &name);

};

// when texture2d is created, it(std::shared_ptr<Texture2D>) is owned by assetmanager.
// then users can hold or release the resources safely
class Texture2D final: public Resource
{
    private:
        GLuint id_;
        int width_;
        int height_;
        // GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA,
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
        Ref_ptr<TextureRegion2D> getTextureRegion();
        inline void bind()const{::glBindTexture(GL_TEXTURE_2D, id_);}
        inline void unbind()const{::glBindTexture(GL_TEXTURE_2D,0);}
};
#endif /* _TEXTURE_MANAGER_HPP */
