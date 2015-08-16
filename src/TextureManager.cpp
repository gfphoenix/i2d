#include <Ref.hpp>
#include <Texture.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>

Ref_ptr<TextureManager> TextureManager::tm_ = nullptr;
TextureManager *TextureManager::getInstance()
{
    if(tm_==nullptr){
        tm_ = MM<TextureManager>::New();
    }
    return tm_.get();
}
Texture2D *Texture2D::create(ResourceManager *manager, const Image &img, const string &name)
{
    auto t = MM<Texture2D>::New();
    t->init(img);
    t->setResourceManager(manager);
    t->setName(name);
    return t;
}

Texture2D *Texture2D::create(ResourceManager *manager, const string &name)
{
    auto img = Image::create(name.c_str());
    if(!img)
        return nullptr;
    return create(manager, img, name);
}

void Texture2D::init(const Image &img)
{
    width_ = img.getWidth();
    height_ = img.getHeight();
    switch(img.getCompType()){
        case CompType::LUMINANCE:
            format_ = GL_LUMINANCE; break;
        case CompType::LUMINANCE_ALPHA:
            format_ = GL_LUMINANCE_ALPHA; break;
        case CompType::RGB:
            format_ = GL_RGB; break;
        case CompType::RGBA:
            format_ = GL_RGBA; break;
        default:
            panic("invalid CompType for image");
    }
    ::glGenTextures(1, &id_);
    CheckGL();
    Assert(id_!=0, "Alloc texture id failed");
    bind();
    CheckGL();
    ::glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format_,
            width_, height_, 0, (GLenum)format_, GL_UNSIGNED_BYTE, img.getData());
    CheckGL();
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
void Texture2D::Dispose()
{
    ::glDeleteTextures(1, &id_);
    id_=0;
}
static iVec2 toSize(RegionDirection d, int w, int h)
{
    switch(d){
        case RegionDirection::BOTTOM:
            return iVec2(w, h);
        case RegionDirection::RIGHT:
            return iVec2(-w, h);
        case RegionDirection::LEFT:
            return iVec2(w, -h);
        case RegionDirection::TOP:
            return iVec2(-w, -h);
        default:
            panic("Invalid RegionRotation value");
    }
    return iVec2();
}
Ref_ptr<TextureRegion2D> Texture2D::getTextureRegion(int offx, int offy, int width, int height, RegionDirection d)
{
    Assert(offx>=0 && offy>=0 && width>=0 && height>=0, "Texture2D::getTextureRegion : all param must >=0");
    Assert(offx<width_ && offy<height_, "too large for offset");
    if(width==0 || offx+width>width_)
        width = width_ - offx;
    if(height==0 || offy+height>height_)
        height = height_ - offy;
    auto r = MM<TextureRegion2D>::New();
    r->texture_ = Ref_ptr<Texture2D>(this);
    r->tl_ = iVec2(offx, offy);
    r->size_ = toSize(d, width, height);
    return Ref_ptr<TextureRegion2D>(r);
}
Ref_ptr<TextureRegion2D> Texture2D::getTextureRegion()
{
    if(self)
        return self;
    auto r = MM<TextureRegion2D>::New();
    r->texture_ = Ref_ptr<Texture2D>(this);
    auto w = getWidth();
    auto h = getHeight();
    r->tl_ = iVec2(0,0);
    r->size_ = iVec2(w,h);
    self = Ref_ptr<TextureRegion2D>(r);
    return self;
}

Texture2D *TextureManager::loadTexture__(const string &name)
{
    Assert(map_.find(name) == map_.cend(), "BUG: Thread problem, exist Texture2D before loadTexture__");
    auto t = Texture2D::create(this, name);
    if(t != nullptr)
        link(t);
    return t;
}
Ref_ptr<Texture2D> TextureManager::loadTexture(const string &name)
{
    auto x = map_.find(name);
    if(x!=map_.cend())
        return Ref_ptr<Texture2D>(static_cast<Texture2D*>(x->second));
    return loadTexture__(name);
}
TextureAtlas *TextureManager::loadTextureAtlas__(const string &name)
{
    Assert(map_.find(name) == map_.cend(), "BUG: Thread problem, exist resource before loadTextureAtlas__");
    auto ta = TextureAtlas::create(name);
    if(ta != nullptr)
        link(ta);
    return ta;
}
Ref_ptr<TextureAtlas> TextureManager::loadTextureAtlas(const string &name)
{
    auto x = map_.find(name);
    if(x!=map_.cend())
        return Ref_ptr<TextureAtlas>(static_cast<TextureAtlas*>(x->second));
    return loadTextureAtlas__(name);
}
// load texture or texture-atlas
Ref_ptr<Resource> TextureManager::loadResource(const string &name)
{
    if(name.size()<1)
        return Ref_ptr<Resource>();
    auto x = map_.find(name);
    if(x != map_.cend())
        return Ref_ptr<Resource>(x->second);
    auto n = name.size() - name.rfind(".atlas");
    Resource *res = n==6 ? static_cast<Resource*>(loadTextureAtlas__(name)) : static_cast<Resource*>(loadTexture__(name));
    return Ref_ptr<Resource>(res);
}
