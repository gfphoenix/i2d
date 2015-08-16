#ifndef STAGELAYER
#define STAGELAYER

#include "types.hpp"
#include "Node.hpp"
#include "Camera.hpp"

enum class ResolutionPolicy
{
    FULL,
    KEEP,
    NO_BOARDER,
};
class Scene;
class Renderer;
// each stage has its own camera
// and resolution policy.
// Stage Layer should has a way to know the screen resolution
class StageLayer : public Node
{
protected:
    Scene *scene_;
    Camera camera_;
    Vec2 designSize_;
    Vec2 viewSize_;
    Vec2 viewOrigin_;
    ResolutionPolicy policy_;
    int prio_;
public:
    StageLayer(const Vec2 &designSize, ResolutionPolicy policy);
    ResolutionPolicy getResolutionPolicy()const{return policy_;}
    inline int getPrio()const{return prio_;}
    inline void setPrio(int prio){prio_=prio;}
    inline void glViewport()const
    {
        ::glViewport(viewOrigin_.x, viewOrigin_.y, viewSize_.x, viewSize_.y);
    }
    const Camera &getCamera()const{return camera_;}
    // screen size
    const Vec2 &getViewSize()const{return viewSize_;}
    const Vec2 &getViewOrigin()const{return viewOrigin_;}

    const Vec2 &getDesignSize()const{return designSize_;}
    virtual void Render(Renderer *renderer);
    virtual void Flush(Renderer *renderer);

};

#endif // STAGELAYER

