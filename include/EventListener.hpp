#ifndef LISTENER_HPP
#define LISTENER_HPP
#include "Node.hpp"
#include <Ref.hpp>
#include <functional>

class Event;
class EventDispatcher;
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
            TOUCH_ONE = 0,
            TOUCH_ALL,
            KEYBOARD,
            MOUSE,
            Type_Number, // number of all event-listeners, exclude this guy
        };

        inline bool isPaused()const{return state_==State::PAUSED;}
        inline void setPaused(bool pause){state_=pause?State::PAUSED:State::RUNNING;}
        inline bool isRegistered()const{return state_!=State::ZERO;}
        inline void setRegistered(){state_=State::RUNNING;}
        inline bool isRunning()const{return state_==State::RUNNING;}
        inline bool isAlive()const{return state_==State::RUNNING||state_==State::PAUSED;}
        inline bool isZombie()const{return state_==State::ZOMBIE;}
        virtual Type getType()const=0;
        virtual void reset();
    protected:
        EventListener();
        virtual ~EventListener(){}
        void init(const std::function<void(Event*)> &fn);
        inline int getFixedPriority()const{return priority_;}
        inline void setFixedPriority(int prio){priority_=prio;}
        inline Node *getNode()const{return node_;}
        inline void setNode(Node *node){node_=node;}
        inline Node *getSceneNodePriority()const{return node_;}
        inline void setSceneNodePriority(Node *node){node_=node;}
        inline void markRemove(){state_=State::ZOMBIE;}
        friend class EventDispatcher;


        std::function<void(Event*)> fn_;
        int priority_;
        Node *node_;
        State state_;
        //bool paused_;
        //bool registered_;
};


#endif // LISTENER_HPP
