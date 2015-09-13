#include <Ref.hpp>
#include <Texture.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>

Ref_ptr<TextureManager> TextureManager::tm_ = nullptr;
TextureManager *TextureManager::getInstance()
{
    if(tm_==nullptr)
        tm_ = MM<TextureManager>::New();
    return tm_.get();
}
Texture2D *Texture2D::create(ResourceManager *manager, const Image_RGBA8 &img, const string &name)
{
    auto t = MM<Texture2D>::New();
    t->init(img);
    t->setResourceManager(manager);
    t->setResourceName(name);
    printf("T2d=%p\n", t);
    return t;
}

Texture2D *Texture2D::create(ResourceManager *manager, const string &name)
{
    auto img = Image_RGBA8::create(name.c_str());
    if(!img)
        return nullptr;
    return create(manager, img, name);
}

void Texture2D::init(const Image_RGBA8 &img)
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
    Assert(id_!=0, "Alloc texture id failed");
    bind();
    ::glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format_,
            width_, height_, 0, (GLenum)format_, GL_UNSIGNED_BYTE, img.getData());
    //CheckGL();
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
/*
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
*/
Ref_ptr<TextureRegion2D> Texture2D::getTextureRegion(int offx, int offy, int width, int height, RegionDirection d)
{
    Assert(offx>=0 && offy>=0 && width>=0 && height>=0, "Texture2D::getTextureRegion : all param must >=0");
    Assert(offx<width_ && offy<height_, "too large for offset");
    if(width==0 || offx+width>width_)
        width = width_ - offx;
    if(height==0 || offy+height>height_)
        height = height_ - offy;
    auto r = MM<TextureRegion2D>::New();
    r->init(this, iVec2(offx,offy), iVec2(width,height), d);
    return Ref_ptr<TextureRegion2D>(r);
}
Ref_ptr<TextureRegion2D> Texture2D::getTextureRegion(RegionDirection dir)
{
    if(self)
        return self;
    auto w = getWidth();
    auto h = getHeight();

    auto r = MM<TextureRegion2D>::New();
    r->init(this, iVec2(0,0), iVec2(w,h), dir);
    r->setDeleteCallback(Texture2D::deleteSelf, this);
    self = r;
    return r;
}
// delete self texture region
void Texture2D::deleteSelf(TextureRegion2D *, void *self)
{
    auto t = static_cast<Texture2D*>(self);
    t->self = nullptr;
}

Texture2D *TextureManager::loadTexture__(const string &name)
{
    Assert(map_.find(name) == map_.cend(), "BUG: Thread problem, exist Texture2D before loadTexture__");
    auto t = Texture2D::create(this, name);
    return t;
}
Ref_ptr<Texture2D> TextureManager::loadTexture(const string &name)
{
    auto x = get(name);
    auto p = x.get();
    auto t = dynamic_cast<Texture2D*>(p);
    return (t);
}
TextureAtlas *TextureManager::loadTextureAtlas__(const std::string &name)
{
    Assert(map_.find(name) == map_.cend(), "BUG: Thread problem, exist resource before loadTextureAtlas__");
    auto ta = TextureAtlas::create(name);
    return ta;
}
Ref_ptr<TextureAtlas> TextureManager::loadTextureAtlas(const std::string &name)
{
    auto x = get(name);
    auto p = x.get();
    auto ta = dynamic_cast<TextureAtlas*>(p);
    return ta;
}
// do loading job : load texture or texture-atlas
Ref_ptr<Resource> TextureManager::loadResource(const std::string &name)
{
    if(name.size()<1)
        return Ref_ptr<Resource>();
    //auto x = map_.find(name);
    //if(x != map_.cend())
    //    return Ref_ptr<Resource>(x->second);
    auto n = name.size();
    if(n>6 && name.compare(n-6, 6, ".atlas")==0)
        return loadTextureAtlas__(name);
    return loadTexture__(name);
}
