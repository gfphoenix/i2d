#ifndef _TEXTURE_MANAGER_HPP
#define _TEXTURE_MANAGER_HPP
#include <gl>
#include <Ref.hpp>
#include <ResourceManager.hpp>
#include <Texture.hpp>
#include <TextureLoader.hpp>
class BMFont;
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
    BMFont *loadBMFontSet__(const std::string &name);
	static Ref_ptr<TextureManager> tm_;
public:
	static TextureManager *getInstance();
    Ref_ptr<Texture2D> loadTexture(const std::string &name);
    Ref_ptr<TextureAtlas> loadTextureAtlas(const std::string &name);
    Ref_ptr<BMFont> loadBMFontSet(const std::string &name);
protected:
    Ref_ptr<Resource> loadResource(const std::string &name)override;
};

#endif /* _TEXTURE_MANAGER_HPP */
