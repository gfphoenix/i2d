#ifndef EVENT_HPP
#define EVENT_HPP
#include "gl"
#include "types.hpp"
#include "input/input.hpp"
#include "EventListener.hpp"
class Node;
class EventListener;
class Event
{
    public:
        enum class Type
        {
            INVALID = -1,
            KEYBOARD = 0,
            CUSTOM,
            MOUSE,
            TOUCH,
        };
        Event();
        virtual ~Event(){}
        virtual bool matchListenerType(EventListener::Type lType)const=0;
        virtual Type getType()const{return Type::INVALID;}
        //no key modifiers by default
        virtual KeyMods getKeyMods()const{return (KeyMods)0;}

        //inline Node *getNode()const{return node_;}
        inline bool isStopped()const{return stopped_;}
        inline void setStop(){stopped_=true;}
    protected:
        //inline void setNode(Node *node){node_=node;}
        //Node *node_;
        bool stopped_;
};
class Touch final
{
    public:
        Touch();

        // all postions are gl positions.
        // i.e. origin is at bottom-left of the screen
        const Vec2 &getLocation()const{return currentPoint_;}
        const Vec2 &getPreviousLocation()const{return previousPoint_;}
        const Vec2 &getStartLocation()const{return startPoint_;}
        Vec2 getDelta()const{return currentPoint_-previousPoint_;}
        // touch id goes from 0 to the max-1
        inline int getID()const{return id_;}
        void setTouchInfo(int id, float x, float y);

    private:
        inline bool hasInit()const{return id_>=0;}
        Vec2 startPoint_;
        Vec2 previousPoint_;
        Vec2 currentPoint_;
        int id_;
};

class EventTouch : public Event
{
    public:
        enum class TouchCode
        {
            TOUCH_BEGAN,
            TOUCH_MOVED,
            TOUCH_ENDED,
            TOUCH_CANCELLED,
        };
        EventTouch();
        inline TouchCode getEventCode()const{return code_;}
        virtual bool matchListenerType(EventListener::Type lType)const override final
        {return lType==EventListener::Type::TOUCH_ALL || lType==EventListener::Type::TOUCH_ONE;}
        inline const std::vector<Touch*> &getTouches()const{return touches_;}
    protected:
        inline void setTouchCode(TouchCode code){code_=code;}
    private:
        TouchCode code_;
        std::vector<Touch*> touches_;
};
enum class MouseButton
{
    LEFT=0,
    RIGHT,
    MIDDLE,
    NR, // number
};
enum class MouseCode
{
    PRESS,
    MOVE,
    RELEASE,
    SCROLL,
};
class StageLayer;
class EventMouse : public Event
{
    public:
        inline EventMouse(MouseCode code):Event(),mouseCode_(code),mods_(KeyMods::MOD_NIL){}
        inline MouseCode getMouseCode()const{return mouseCode_;}
        Event::Type getType()const override final{return Event::Type::MOUSE;}
        virtual bool matchListenerType(EventListener::Type lType)const override final
        {return lType==EventListener::Type::MOUSE;}
        KeyMods getKeyMods()const override final{return mods_;}
        inline MouseButton getMouseButton()const{return button_;}
        // valid for left or right button
        // screen pos start from bottom left of window
        inline const Vec2 &getCursorPos()const{return pos1_;}
        inline float getCursorX()const{return pos1_.x;}
        inline float getCursorY()const{return pos1_.y;}
        inline const Vec2 &getCursorWorld()const{return world1_;}
        inline const Vec2 &getStartPos()const{return pos0_;}
        //only valid for middle button
        inline const Vec2 &getScroll()const{return scroll_;}
        inline float getScrollX()const{return scroll_.x;}
        inline float getScrollY()const{return scroll_.y;}
    protected:
        inline void setMouseButton(MouseButton button){button_=button;}
        inline void setKeyMods(KeyMods mods){mods_=mods;}
        inline void setStartPos(const Vec2 &pos){pos0_=pos;}
        inline void setCursorPos(float x, float y){pos1_.x=x;pos1_.y=y;}
        inline void setCursorPos(const Vec2 &pos){pos1_=pos;}
        inline void setCursorWorld(float x, float y){world1_.x=x;world1_.y=y;}
        inline void setCursorWorld(const Vec2 &worldPos){world1_=worldPos;}
        inline void setScroll(float scollX, float scrollY){scroll_.x=scollX;scroll_.y=scrollY;}
        inline void setScroll(const Vec2 &scroll){scroll_=scroll;}

        MouseButton button_;
        MouseCode mouseCode_;
        KeyMods mods_;
        Vec2 pos0_;
        Vec2 pos1_, world1_;
        Vec2 scroll_;

        friend class EventMouseListener;
        friend class StageLayer;
};

enum class KeyAction
{
    KEY_PRESS,
    KEY_RELEASE,
};
class EventKeyboard : public Event
{
    public:
        Event::Type getType()const{return Event::Type::KEYBOARD;}
        virtual bool matchListenerType(EventListener::Type lType)const override final
        {return lType==EventListener::Type::KEYBOARD;}

        KeyMods getKeyMods()const override final{return mods_;}
        inline KeyCode getKeyCode()const{return code_;}
        inline KeyAction getKeyAction()const{return action_;}
    protected:
        KeyCode code_;
        KeyMods mods_; // Ctrl | Alt | Shift | Super
        KeyAction action_;
};

#endif // EVENT_HPP
