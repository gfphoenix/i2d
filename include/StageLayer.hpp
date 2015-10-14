#ifndef STAGELAYER
#define STAGELAYER

#include "types.hpp"
#include "Node.hpp"
#include "Camera.hpp"
#include "Event.hpp"
#include "EventListener.hpp"
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
        // event-listener
        // make sure that mouse cursor and touch position in the viewport
        // defined by viewOrigin_ and viewSize_
        std::vector<Ref_ptr<EventListener>> sceneListeners_[(int)EventListener::Type::Type_Number];
        std::pair<KeyCode, Ref_ptr<EventListener>> keySlots_[8];
        Ref_ptr<EventListener> mouseSlots_[(int)MouseButton::NR];
        unsigned dirty_listenerOrder_:1;// default to false
        unsigned enable_event_:1; // default to true
        friend class Scene;
    public:
        StageLayer(const Vec2 &designSize, ResolutionPolicy policy=ResolutionPolicy::FULL);
        virtual Scene *getScene()override{return scene_;}
        virtual StageLayer *getStageLayer()override{return this;}
        void onWinSizeChanged();
        inline ResolutionPolicy getResolutionPolicy()const{return policy_;}
        inline void setNeedOrderListeners(){dirty_listenerOrder_=true;}
        inline bool isNeedOrderListeners()const{return dirty_listenerOrder_;}
        inline void setViewport()const
        {
            ::glViewport((int)viewOrigin_.x, (int)viewOrigin_.y, (int)viewSize_.x, (int)viewSize_.y);
        }
        inline bool inView(float screenX, float screenY)const
        {
            return screenX>=viewOrigin_.x && screenX<=viewOrigin_.x+viewSize_.x &&
                screenY>=viewOrigin_.y && screenY<=viewOrigin_.y+viewSize_.y;
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

    protected:
        void clearListeners();
        inline void updateStageListeners()
        {
            if(dirty_listenerOrder_){
                clearListeners();
                visitNode__(this);
                dirty_listenerOrder_=false;
            }
        }

        void visitNode__(Node *node);
        virtual void beforeEnter();
        virtual void beforeExit();
        virtual void afterEnter();
        virtual void afterExit();

        // handle event
        virtual bool handleEvent(Event *);
        virtual bool dispatchEvent(Event *);
        virtual bool handleEventMouse(EventMouse*);
        virtual bool handleEventKeyboard(EventKeyboard *e);
        bool foundListener(Event *e, EventListener *l);
        int findEmptyKeySlot()const;
        int findKeySlot(KeyCode code)const;
};

#endif // STAGELAYER
