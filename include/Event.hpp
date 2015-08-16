#ifndef EVENT_HPP
#define EVENT_HPP

class Node;
class Event
{
    public:
        enum class Type
        {
            INVALID,
            TOUCH,
            MOUSE,
            KEYBORAD,
        };
        Event();
        virtual ~Event(){}
        virtual Type getType()const=0;
        inline Node *getNode()const{return node_;}
        inline bool isStopped()const{return stopped_;}
        inline void setStop(){stopped_=true;}
    protected:
        inline void setNode(Node *node){node_=node;}
    private:
        Node *node_;
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
        inline const std::vector<Touch*> &getTouches()const{return touches_;}
    protected:
        inline void setTouchCode(TouchCode code){code_=code;}
    private:
        TouchCode code_;
        std::vector<Touch*> touches_;
};
class EventMouse : public Event
{
    public:
        enum class MouseCode
        {
            MOUSE_LEFT_DOWN,
            MOUSE_LEFT_MOVE,
            MOUSE_LEFT_UP,
            MOUSE_RIGHT_DOWN,
            MOUSE_RIGHT_MOVE,
            MOUSE_RIGHT_UP,
            MOUSE_SCROLL,
        };
        EventMouse(MouseCode code);
        MouseCode getMouseCode()const{return mouseCode_;}
        Event::Type getType()const final{return Event::Type::MOUSE;}
        inline int getMouseButton(){return button_;}
        inline const Vec2 &getCursorPosition()const{return cursor_;}
        inline float getCursorX()const{return cursor_.x;}
        inline float getCursorY()const{return cursor_.y;}
        inline const Vec2 &getScroll()const{return scroll_;}
        inline float getScrollX()const{return scroll_.x;}
        inline float getScrollY()const{return scroll_.y;}
    protected:
        inline void setMouseButton(int button){button_=button;}
        inline void setCursorPosition(float x, float y){cursor_.x=x;cursor_.y=y;}
        inline void setScroll(float scollX, float scrollY){scroll_.x=scollX;scroll_.y=scrollY;}

    private:
        MouseCode mouseCode_;
        int button_;
        Vec2 cursor_;
        Vec2 scroll_;
};

enum class KeyAction
{
    KEY_PRESS,
    KEY_RELEASE,
};
#include "GLView_linux.hpp"
class EventKeyboard final : public Event
{
    public:
        Event::Type getType()const{return Event::Type::KEYBORAD;}
        KeyCode code;
        KeyMods mods; // Ctrl | Alt | Shift | Super
        KeyAction action;
};

#endif // EVENT_HPP
