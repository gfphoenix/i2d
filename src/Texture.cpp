#include <types.hpp>
#include <Texture.hpp>
#include <mm.hpp>
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
Ref_ptr<TextureRegion2D> TextureRegion2D::getSubRegion(int offx, int offy, int width, int height)const
{
    auto w = getWidth();
    auto h = getHeight();
    Assert(offx>=0 && offy>=0 && width>=0 && height>=0,
            "Negative param in TextureRegion2D::getSubRegion()");
    Assert(offx<w && offy<h,
            "offset is larger than region size");
    if(width==0 || offx+width>w){
        Debug("too large size in TextureRegion2D::getSubRegion\n");
        width = w - offx;
    }
    if(height==0 || offy+height>h){
        Debug("too large size in TextureRegion2D::getSubRegion\n");
        height = h - offy;
    }
    auto bits = 0;
    auto r = MM<TextureRegion2D>::New();
    r->texture_ = texture_;
    if(size_.x<0){ // 底在右侧
        bits |= 1;
    }
    if(size_.y<0){ // 底在左侧
        bits |= 2;
    }
    switch(bits){
        case 0:
            r->tl_ = iVec2(tl_.x + offx, tl_.y + offy);
            r->size_ = iVec2(width, height);
            break;
        // 底在右侧
        case 1:
            r->tl_ = iVec2(tl_.x+offy, tl_.y+size_.y-offx-width);
            r->size_ = iVec2(-width, height);
            break;
        // 底在左侧
        case 2:
            r->tl_ = iVec2(tl_.x+size_.x-offy-height, tl_.y+offx);
            r->size_ = iVec2(width, -height);
            break;
        // 底在上侧，size_.x<0 & size_.y<0
        case 3:
            r->tl_ = iVec2(tl_.x+w-offx-width, tl_.y+h-offy-height);
            r->size_ = iVec2(-width, -height);
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
    auto w = getWidth();
    auto h = getHeight();
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
Ref_ptr<TextureRegion2D> TextureAtlas::get(const string &name)const
{
    auto x = regions_.find(name);
    if(x == regions_.cend())
        return Ref_ptr<TextureRegion2D>();
    return x->second;
}
AtlasConfig TextureAtlas::parseAtlasConfig(const string &atlasName)
{
    do{}while(!sizeof(atlasName));
    return AtlasConfig();
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
    auto ta = create(parseAtlasConfig(atlasName));
    ta->setResourceManager(TextureManager::getInstance());
    ta->setName(atlasName);
    return ta;
}
