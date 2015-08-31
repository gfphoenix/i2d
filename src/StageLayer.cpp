#include <App.hpp>
#include <Director.hpp>
#include <Renderer.hpp>
#include <Camera.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>

StageLayer::StageLayer(const Vec2 &designSize, ResolutionPolicy policy):
    Node(), scene_(nullptr), designSize_(designSize), policy_(policy)
{
    stage_ = this;
    this->setAnchor(0,0);
    auto half = designSize_*.5f;
    camera_.setEye__(half);
    camera_.ortho2d__(-half.x, half.x, -half.y, half.y);
    onWinSizeChanged();
}
void StageLayer::onWinSizeChanged()
{
    auto sz = Director::getInstance()->getWinSize();
    switch (policy_) {
        case ResolutionPolicy::FULL: 
            {
                viewOrigin_ = Vec2(0,0);
                viewSize_ = sz;
                printf("view size=(%f x %f)\n", sz.x, sz.y);
                break;
            }
        case ResolutionPolicy::KEEP: 
            {
                auto sx = sz.x / designSize_.x;
                auto sy = sz.y / designSize_.y;
                if (sx < sy) {//屏幕太窄，上下两边黑边
                    viewSize_ = Vec2(sz.x, sz.x * designSize_.y / designSize_.x);
                    viewOrigin_ = Vec2(0, (sz.y - viewSize_.y)/2);
                }else{//屏幕太宽，左右两边黑边
                    viewSize_   = Vec2(sz.y * designSize_.x / designSize_.y, sz.y);
                    viewOrigin_ = Vec2((sz.x - viewSize_.x)/2, 0);
                }
                break;
            }
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
                break;
            }
        default:
            break;
    }
    camera_.update();
}

Vec2 StageLayer::screenToWorld(float screenX, float screenY)
{
    auto const &pv_1 = camera_.getPV_1();
    // transform screen (screenX, screenY) => [0, 1]x[0,1] => [-1,-1]x[-1,1]
    //auto x = (screenX - viewOrigin_.x) / viewSize_.x;
    //auto y = (screenY - viewOrigin_.y) / viewSize_.y;
    // transform [0,1]x[0,1] to [-1, -1]x[-1,1]
    // x = x * 2 - 1;
    // y = y * 2 - 1;
    auto x = (screenX - viewOrigin_.x) *2/ viewSize_.x -1;
    auto y = (screenY - viewOrigin_.y) *2/ viewSize_.y -1;

    auto P = pv_1 * Vec4(x, y, 0, 1);
    if (P.w != 1.f)
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
    this->visit(renderer, Mat3(), false);
    renderer->Flush();
}
void StageLayer::clearListeners()
{
    for(auto &x : sceneListeners_)
        x.clear();
}

void StageLayer::visitNode__(Node *node)
{
    node->sortChildrenOrder();
    auto &children = node->getChildren();
    // children is sorted by zIndex: -1, 0, 1
    int i=(int)children.size()-1;
    Node *tmp;
    while(i>=0 && (tmp=children[i].get()) && tmp->getZindex()>=0){
        visitNode__(tmp);
        i--;
    }
    {// append listeners to the stage
        if(node->hasListeners()){
            auto const &v = node->getEventListeners();
            for(auto const &l : v){
                int idx = (int)l->getType();
                auto &vv = sceneListeners_[idx];
                vv.push_back(l);
            }
        }
    }
    while(i>=0){
        visitNode__(children[i--].get());
    }
}
bool StageLayer::handleEvent(Event *e)
{
    switch(e->getType()){
        case Event::Type::KEYBOARD:
            return handleEventKeyboard(static_cast<EventKeyboard*>(e));
        case Event::Type::MOUSE:
            return handleEventMouse(static_cast<EventMouse*>(e));
        case Event::Type::TOUCH:
        case Event::Type::INVALID:
        default:
            break;
    }

    return false;
}
bool StageLayer::handleEventKeyboard(EventKeyboard *e)
{
    switch (e->getKeyAction()) {
        case KeyAction::KEY_PRESS:
            return dispatchEvent(e);
        case KeyAction::KEY_RELEASE:
            {
                auto idx = findKeySlot(e->getKeyCode());
                if(idx<0)
                    break;
                auto l = static_cast<EventKeyboardListener*>(keySlots_[idx].second.get());
                if(l->isRunning() && l->onRelease){
                    l->onRelease(e);
                }
                keySlots_[idx].second = nullptr;
                return true;
            }
    }
    return false;
}
bool StageLayer::handleEventMouse(EventMouse *e)
{
    auto btn = e->getMouseButton();
    auto idx = (int)btn;
    switch(e->getMouseCode()){
        case MouseCode::PRESS:
            {
                auto const &pos = e->getCursorPos();
                if(!inView(pos.x, pos.y))
                    return false;

                auto worldPos = screenToWorld(pos);
                e->setCursorWorld(worldPos.x, worldPos.y);

                return dispatchEvent(e);
            }
        case MouseCode::RELEASE:
            {
                auto l = static_cast<EventMouseListener*>(mouseSlots_[idx].get());
                auto ok = l && l->isRunning();
                if(ok && l->onRelease)
                    l->onRelease(e);
                mouseSlots_[idx] = nullptr;
                return ok;
            }
        case MouseCode::MOVE:
            {
                auto l = static_cast<EventMouseListener*>(mouseSlots_[idx].get());
                auto ok = l && l->isRunning();
                if(ok && l->onMove)
                    l->onMove(e);
                return ok;
            }
            // FIX: no need to start with press ok
        case MouseCode::SCROLL:
            {
                auto l = static_cast<EventMouseListener*>(mouseSlots_[idx].get());
                auto ok = l && l->isRunning();
                if(ok && l->onScroll)
                    l->onScroll(e);
                return ok;
            }
    }
    return false;
}
int StageLayer::findEmptyKeySlot()const
{
    int n = sizeof(keySlots_)/sizeof(keySlots_[0]);
    while(n-->0){
        auto const &slot = keySlots_[n];
        if(slot.second==nullptr)
            return n;
    }
    return -1;
}
int StageLayer::findKeySlot(KeyCode code) const
{
    int i=(int)(sizeof(keySlots_)/sizeof(keySlots_[0]));
    while(i-->0){
        auto const &x = keySlots_[i];
        if(x.first==code && x.second!=nullptr)
            return i;
    }
    return -1;
}

bool StageLayer::foundListener(Event *e, EventListener *l)
{
    switch (e->getType()) {
        case Event::Type::KEYBOARD:
            {
                auto ev = static_cast<EventKeyboard*>(e);
                auto idx = findEmptyKeySlot();
                if(idx>=0){//if can't find a slot, just eat it
                    keySlots_[idx] = {ev->getKeyCode(), l};
                }
                break;
            }
        case Event::Type::MOUSE:
            {
                auto ev = static_cast<EventMouse*>(e);
                auto idx = (int)ev->getMouseButton();
                mouseSlots_[idx] = l;
                break;
            }
        default:
            break;
    }
    return true;
}

bool StageLayer::dispatchEvent(Event *e)
{
    EventListener *L=nullptr;
    /*
       auto it = fixedListeners_.rbegin();
       auto end = fixedListeners_.rend();
       e->onDispatchBegin();
       while(it!=end){
       auto l = static_cast<EventListener*>(it->get());
       if(l->getFixedPriority()<0)
       break;
       if(e->matchListenerType(l->getType()) && l->isRunning() && l->handle(e)){
       return;
       }
       ++it;
       }
       */
    //scene
    auto &v = sceneListeners_[(int)e->getType()];
    for(auto sit=v.begin(); sit!=v.end(); ++sit){
        auto l = sit->get();
        if(l->isRunning() && l->handle(e)){
            L = l;
            goto found;
        }
    }
    /*
       while(it!=end){
       auto l = static_cast<EventListener*>(it->get());
       if(e->matchListenerType(l->getType()) && l->isRunning() && l->handle(e)){
       return;
       }
       ++it;
       }
       */

    return false;
found: // trace the trigger event
    return foundListener(e, L);
}
void StageLayer::beforeEnter(){}
void StageLayer::beforeExit(){}
void StageLayer::afterEnter(){}
void StageLayer::afterExit(){}
