#ifndef LISTENER_HPP
#define LISTENER_HPP
#include <Ref.hpp>
#include <functional>

class Event;
class EventKeyboard;
class EventMouse;
class EventDispatcher;
class Node;
class EventListener : public Ref
{
    public:
        // Listener's Type will be used as an index in EventDispatcher
        enum class State
        {
            ZERO,   // not added to the dispatcher
            RUNNING,
            PAUSED,
            ZOMBIE, // going to be removed
        };
        enum class Type
        {
            UNKNOWN = -1,
            KEYBOARD = 0,
            CUSTOM,
            MOUSE,
            TOUCH_ONE,
            TOUCH_ALL,

            Type_Number, // number of all event-listeners, exclude this guy
        };

        inline bool isPaused()const{return state_==State::PAUSED;}
        inline void setPaused(bool pause){state_=pause?State::PAUSED:State::RUNNING;}
        inline bool isRegistered()const{return state_!=State::ZERO;}
        inline void setRegistered(){state_=State::RUNNING;}
        inline bool isRunning()const{return state_==State::RUNNING;}
        // running or paused
        inline bool isAlive()const{return state_==State::RUNNING||state_==State::PAUSED;}
        virtual Type getType()const=0;
        virtual bool isValid()const=0;
        inline int getFixedPriority()const{return priority_;}
        inline Node *getNode()const{return node_;}
        virtual void reset();

        // for first action, return true means "I will handle it"
        // and set the dispatch state of EventDispatcher if matched
        virtual bool handle(Event *)=0;
    protected:
        EventListener();
        virtual ~EventListener(){}
        inline void setFixedPriority(int prio){priority_=prio;}
        inline void setNode(Node *node){node_=node;}
        inline void setDead(){state_=State::ZOMBIE;}//when node removed
        friend class EventDispatcher;
        friend class ListenerVector;
        friend class Node;

        Node *node_;
        int priority_;
        State state_;
};

class EventMouseListener : public EventListener
{
public:
    bool handle(Event *)override;
    virtual Type getType()const final{return EventListener::Type::MOUSE;}
    virtual bool isValid()const final{return state_==State::ZERO && (onPress!=nullptr || onScroll!=nullptr);}
    std::function<bool(EventMouse *)> onPress;
    std::function<void(EventMouse *)> onMove;
    std::function<void(EventMouse *)> onRelease;
    std::function<bool(EventMouse *)> onScroll;
};

class EventKeyboardListener : public EventListener
{
public:
    bool handle(Event *)override;
    virtual Type getType()const final{return EventListener::Type::KEYBOARD;}
    virtual bool isValid()const final{return state_==State::ZERO && onPress!=nullptr;}
    std::function<bool(EventKeyboard *)> onPress;
    std::function<void(EventKeyboard *)> onRelease;
};

#endif // LISTENER_HPP
