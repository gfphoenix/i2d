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
    friend class Scene;
public:
    StageLayer(const Vec2 &designSize, ResolutionPolicy policy);
    virtual Scene *getScene()override{return scene_;}
    virtual StageLayer *getStageLayer()override{return this;}
    void onWinSizeChanged();
    inline ResolutionPolicy getResolutionPolicy()const{return policy_;}
    inline int getPrio()const{return prio_;}
    inline void setPrio(int prio){prio_=prio;}
    inline void setViewport()const
    {
        ::glViewport((int)viewOrigin_.x, (int)viewOrigin_.y, (int)viewSize_.x, (int)viewSize_.y);
    }
    inline const Camera &getCamera()const{return camera_;}
    inline Camera &getCamera(){return camera_;}
    // screen size
    const Vec2 &getViewSize()const{return viewSize_;}
    const Vec2 &getViewOrigin()const{return viewOrigin_;}
    Vec2 screenToWorld(float screenX, float screenY);
    inline Vec2 screenToWorld(const Vec2 &screen)
    {return screenToWorld(screen.x,screen.y);}

    const Vec2 &getDesignSize()const{return designSize_;}
    virtual void Render(Renderer *renderer);
    virtual void beforeEnter()override;
};

#endif // STAGELAYER
