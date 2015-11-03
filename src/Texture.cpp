#include <types.hpp>
#include <Texture.hpp>
#include <mm.hpp>

std::vector<std::pair<std::string,loadTA_func_t>> TextureAtlas::loaders_;
void TextureAtlas::registerLoader(const char *suffix, loadTA_func_t fn)
{
    loaders_.push_back({{suffix, fn}});
}
RegionDirection TextureRegion2D::getRegionDirection()const
{
    int bits = (int)RegionDirection::BOTTOM;
    if(size_.x<0){ // 底在右侧
        bits |= (int)RegionDirection::RIGHT;
    }
    if(size_.y<0){ // 底在左侧
        bits |= (int)RegionDirection::LEFT;
    }
    return (RegionDirection)bits;
}
void TextureRegion2D::setDeleteCallback(void (*fn)(TextureRegion2D *,void*), void*data)
{
    this->onDestroy = fn;
    this->destroyData_ = data;
}
void TextureRegion2D::Delete()
{
    if(onDestroy)
        onDestroy(this, destroyData_);
    Ref::Delete();
}
// size is a positive integer pair
void TextureRegion2D::init(Texture2D *tex, const iVec2 &tl, const iVec2 &size, RegionDirection dir)
{
    texture_ = tex;
    this->tl_ = tl;
    float w = tex->getWidth();
    float h = tex->getHeight();
    this->uv_tl_.x = tl.x / w;
    this->uv_tl_.y = tl.y / h;
    this->uv_br_.x = (tl.x+size.x) / w;
    this->uv_br_.y = (tl.y+size.y) / h;
    switch(dir){
        case RegionDirection::BOTTOM:
            this->size_ = size;
            break;
        case RegionDirection::RIGHT:
            this->size_.x = -size.x;
            this->size_.y =  size.y;
            break;
        case RegionDirection::LEFT:
            this->size_.x = size.x;
            this->size_.y = -size.y;
            break;
        case RegionDirection::TOP:
            this->size_.x = -size.x;
            this->size_.y = -size.y;
            break;
        default:
            break;
    }
}

Ref_ptr<TextureRegion2D> TextureRegion2D::getSubRegion(int offx, int offy, int width, int height, RegionDirection dir)const
{
    auto w = getRegionWidth();
    auto h = getRegionHeight();
    Assert(offx>=0 && offy>=0 && width>=0 && height>=0,
            "Negative param in TextureRegion2D::getSubRegion()");
    Assert(offx<w && offy<h,
            "offset is larger than region size");
    if(offx+width>w){
        Debug("too large size in TextureRegion2D::getSubRegion\n");
        width = w - offx;
    }
    if(offy+height>h){
        Debug("too large size in TextureRegion2D::getSubRegion\n");
        height = h - offy;
    }

    auto r = MM<TextureRegion2D>::New();
    r->texture_ = texture_;
    auto selfDir =getRegionDirection();
    auto tex = texture_.get();
    RegionDirection tmp;
    switch(selfDir){
        case RegionDirection::BOTTOM:
        // mother's dir is bottom
            r->init(tex, iVec2(tl_.x+offx, tl_.y+offy), iVec2(width, height), dir);
        break;
        // mother's dir is right 底在右侧
        case RegionDirection::RIGHT:
            switch (dir) {
            case RegionDirection::BOTTOM:
                tmp = RegionDirection::RIGHT; break;
            case RegionDirection::TOP:
                tmp = RegionDirection::LEFT; break;
            case RegionDirection::LEFT:
                tmp = RegionDirection::BOTTOM; break;
            case RegionDirection::RIGHT:
                tmp = RegionDirection::TOP;  break;
            default:
                break;
            }
            r->init(tex, iVec2(tl_.x+offy, tl_.y+h-offx-width), iVec2(height, width), tmp);
            break;
        // 底在左侧
        case RegionDirection::LEFT:
            switch (dir) {
            case RegionDirection::BOTTOM:
                tmp = RegionDirection::LEFT; break;
            case RegionDirection::TOP:
                tmp = RegionDirection::RIGHT; break;
            case RegionDirection::LEFT:
                tmp = RegionDirection::TOP; break;
            case RegionDirection::RIGHT:
                tmp = RegionDirection::BOTTOM; break;
            default:
                break;
            }
            r->init(tex, iVec2(tl_.x+h-offy-height, tl_.y+offx), iVec2(height, width), tmp);
            break;
        // 底在上侧，size_.x<0 & size_.y<0
        case RegionDirection::TOP:
            switch (dir) {
            case RegionDirection::BOTTOM:
                tmp = RegionDirection::TOP; break;
            case RegionDirection::TOP:
                tmp = RegionDirection::BOTTOM; break;
            case RegionDirection::LEFT:
                tmp = RegionDirection::RIGHT; break;
            case RegionDirection::RIGHT:
                tmp = RegionDirection::LEFT; break;

            default:
                break;
            }
            r->init(tex, iVec2(tl_.x+w-offx-width, tl_.y+h-offy-height), iVec2(width, height), tmp);
            break;
    }
    return Ref_ptr<TextureRegion2D>(r);
}
UVi TextureRegion2D::getUVi()const
{
    UVi t;
    auto bits=0;
    if(size_.x<0){ // 底在右侧
        bits |= 1;
    }
    if(size_.y<0){ // 底在左侧
        bits |= 2;
    }
    auto w = getRegionWidth();
    auto h = getRegionHeight();
    switch(bits){
        case 0:
            t.uv[2] = tl_;
            t.uv[0] = iVec2(tl_.x, tl_.y+h);
            t.uv[1] = iVec2(tl_.x+w, tl_.y+h);
            t.uv[3] = iVec2(tl_.x+w, tl_.y);
            break;
        case 1:
            t.uv[0] = tl_;
            t.uv[1] = iVec2(tl_.x, tl_.y+w);
            t.uv[2] = iVec2(tl_.x+h, tl_.y);
            t.uv[3] = iVec2(tl_.x+h, tl_.y+w);
            break;
        case 2:
            t.uv[3] = tl_;
            t.uv[0] = iVec2(tl_.x+h, tl_.y+w);
            t.uv[1] = iVec2(tl_.x+h, tl_.y);
            t.uv[2] = iVec2(tl_.x, tl_.y+w);
            break;
        case 3:
            t.uv[1] = tl_;
            t.uv[0] = iVec2(tl_.x+w, tl_.y);
            t.uv[2] = iVec2(tl_.x+w, tl_.y+h);
            t.uv[3] = iVec2(tl_.x, tl_.y+h);
            break;
    }
    return t;
}
UV TextureRegion2D::getUV()const
{
    auto &&i = getUVi();
    auto w = texture_->getWidth();
    auto h = texture_->getHeight();

    return UV{
        Vec2((double)i.uv[0].x/w, (double)i.uv[0].y/h),
        Vec2((double)i.uv[1].x/w, (double)i.uv[1].y/h),
        Vec2((double)i.uv[2].x/w, (double)i.uv[2].y/h),
        Vec2((double)i.uv[3].x/w, (double)i.uv[3].y/h),
    };
}
void TextureRegion2D::getUV(Vec2 &bl, Vec2 &br, Vec2 &tl, Vec2 &tr)const
{
    auto &&i = getUVi();
    auto w = texture_->getWidth();
    auto h = texture_->getHeight();
    bl.x = (double)i.uv[0].x/w;
    bl.y = (double)i.uv[0].y/h;
    br.x = (double)i.uv[1].x/w;
    br.y = (double)i.uv[1].y/h;
    tl.x = (double)i.uv[2].x/w;
    tl.y = (double)i.uv[2].y/h;
    tr.x = (double)i.uv[3].x/w;
    tr.y = (double)i.uv[3].y/h;
}
Ref_ptr<TextureRegion2D> TextureAtlas::get(const string &name)const
{
    auto x = regions_.find(name);
    if(x == regions_.cend())
        return Ref_ptr<TextureRegion2D>();
    return x->second;
}
static bool match(const std::string &suffix, const std::string &filename)
{
    if(suffix=="*")
        return true;
    auto L1=suffix.size();
    auto L2=filename.size();
    if(L1>=L2)
        return false;
    return filename.compare(L2-L1, L1, suffix)==0;
}
AtlasConfig TextureAtlas::parseAtlasConfig(const string &atlasName)
{
    return AtlasConfig();
}
AtlasConfig TextureAtlas::parseAtlasConfig(const Buffer &buffer)
{
    AtlasConfig config;
    for(auto const &x : loaders_){
        if(match(x->first, atlasName) && x->second(config, buffer))
            break;
    }
    return config;
}
TextureAtlas *TextureAtlas::create(const AtlasConfig &config)
{
    for(auto it=config.config_.cbegin(); it!=config.config_.cend(); it++){
    }
    return nullptr;
}
// load texture-region
TextureAtlas *TextureAtlas::create(const string &atlasName)
{
    //auto ta = create(parseAtlasConfig(atlasName));
    //return ta;
    return nullptr;
}
