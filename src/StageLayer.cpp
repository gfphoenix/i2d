#include <Renderer.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>
static Vec2 getScreenResolution()
{
    return Vec2(1920, 1080);
}

StageLayer::StageLayer(const Vec2 &designSize, ResolutionPolicy policy):
    Node(), designSize_(designSize), policy_(policy), prio_(0)
{
    this->setAnchor(0,0);
    auto sz = getScreenResolution();
    auto half = designSize/2;
    camera_.setEye(half);
    camera_.Orth2d(-half.x, half.x, -half.y, half.y);
    switch (policy) {
    case ResolutionPolicy::FULL:
        viewOrigin_ = Vec2(0,0);
        viewSize_ = sz;
        break;
    case ResolutionPolicy::KEEP:
    {
        auto f1 = sz.x / sz.y;
        auto f2 = designSize.x / designSize.y;
        if(f1<f2){//屏幕太宽，左右两边黑边
            double w = sz.y * designSize.x / designSize.y;
            double x = (sz.x-w)/2;
            viewOrigin_     = Vec2(x, 0);
            viewSize_       = Vec2(w, sz.y);
        }else{
            double h = sz.x * designSize.y / designSize.x;
            double y = (sz.y-h)/2;
            viewOrigin_ = Vec2(0, y);
            viewSize_   = Vec2(sz.x, h);
        }
    }
    case ResolutionPolicy::NO_BOARDER:
    {
        auto f1 = sz.x / sz.y;
        auto f2 = designSize.x / designSize.y;
        if(f1 < f2){//屏幕太窄，上下两边部分被截断
            double w = sz.y * designSize.x / designSize.y;
            double x = (sz.x-w)/2;
            viewOrigin_ = Vec2(x, 0);
            viewSize_   = Vec2(w, sz.y);
        }else{
            double h = sz.y * designSize.y / designSize.x;
            double y = (sz.y-h)/2;
            viewOrigin_ = Vec2(0, y);
            viewSize_   = Vec2(sz.x, h);
        }
    }
    default:
        break;
    }
    // glViewport(viewOrigin_.x, viewOrigin_.y, viewSize_.x, viewSize_.y)
    camera_.update();
}
void StageLayer::Render(Renderer *renderer)
{
    auto const &pv = camera_.getPV();
    // shader set pv
    // render
    camera_.glViewport();
    this->visit(this, Mat3(), false);
}
void StageLayer::Flush(Renderer *renderer)
{
    renderer->Flush(this);
}
