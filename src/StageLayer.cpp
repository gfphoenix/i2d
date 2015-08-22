#include <App.hpp>
#include <Director.hpp>
#include <Renderer.hpp>
#include <Camera.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>

StageLayer::StageLayer(const Vec2 &designSize, ResolutionPolicy policy):
    Node(), designSize_(designSize), policy_(policy), prio_(0)
{
    this->setAnchor(0,0);
    auto half = designSize_*.5f;
    camera_.setEye__(half);
    camera_.ortho2d__(-half.x, half.x, -half.y, half.y);
    onWinSizeChanged();
}
void StageLayer::onWinSizeChanged()
{
    auto sz = Director::getInstance()->getWinSize();
    //auto half = designSize_*.5f;
    //camera_.setEye__(half);
    //camera_.ortho2d__(-half.x, half.x, -half.y, half.y);
    switch (policy_) {
    case ResolutionPolicy::FULL: {
        viewOrigin_ = Vec2(0,0);
        viewSize_ = sz;
        printf("view size=(%f x %f)\n", sz.x, sz.y);
    }
    break;
    case ResolutionPolicy::KEEP: {
        auto sx = sz.x / designSize_.x;
        auto sy = sz.y / designSize_.y;
        if (sx < sy) {//屏幕太窄，上下两边黑边
            viewSize_ = Vec2(sz.x, sz.x * designSize_.y / designSize_.x);
            viewOrigin_ = Vec2(0, (sz.y - viewSize_.y)/2);
        }else{//屏幕太宽，左右两边黑边
            viewSize_   = Vec2(sz.y * designSize_.x / designSize_.y, sz.y);
            viewOrigin_ = Vec2((sz.x - viewSize_.x)/2, 0);
        }
    }
    break;
    case ResolutionPolicy::NO_BOARDER:
    {
        // origin is negative on one side
        // view size is larger than screen size
        // FIX ME !!!
        auto sx = sz.x / designSize_.x;
        auto sy = sz.y / designSize_.y;
        if (sx > sy) {
            viewSize_ = Vec2(sz.x, sz.x * designSize_.y / designSize_.x);
            viewOrigin_ = Vec2(0, (sz.y - viewSize_.y)/2);
        }else{
            viewSize_   = Vec2(sz.y * designSize_.x / designSize_.y, sz.y);
            viewOrigin_ = Vec2((sz.x - viewSize_.x)/2, 0);
        }
        printf("origin=(%f, %f), size=%f x %f\n",
               viewOrigin_.x, viewOrigin_.y,
               viewSize_.x, viewSize_.y
               );
    }
    break;
    default:
        break;
    }
    printf("Before update...\n");
    camera_.update();
    printf("After update...\n");
}

Vec2 StageLayer::screenToWorld(float screenX, float screenY)
{
    auto const &pv_1 = camera_.getPV_1();
    //auto tmp = Vec2(screenX, screenY);
    // transform screen in [0, 1]x[0,1]
    auto x = (screenX - viewOrigin_.x) / viewSize_.x;
    auto y = (screenY - viewOrigin_.y) / viewSize_.y;
    // transform [0,1]x[0,1] to [-1, -1]x[-1,1]
    x = x * 2 - 1;
    y = y * 2 - 1;
    auto P = pv_1 * Vec4(x, y, 0, 1);
    if (P.w != 1)
        P /= P.w;

    return Vec2(P);
}
void StageLayer::Render(Renderer *renderer)
{
    // render
    if(!isVisible())
        return;
    renderer->setStageLayer(this);
    this->setViewport();
    //printf("view rect = (%d, %d) - %d x %d\n",
    //       (int)viewOrigin_.x, (int)viewOrigin_.y,
    //       (int)viewSize_.x, (int)viewSize_.y);
    //auto ws = Director::getInstance()->getWinSize();
    //printf("screen size = %d x %d\n",
    //       (int)ws.x, (int)ws.y
    //       );
    this->visit(renderer, Mat3(), false);
    renderer->Flush();
}
void StageLayer::beforeEnter()
{
    stage_ = this;
}
