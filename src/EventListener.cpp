#include "EventListener.hpp"
#include "Event.hpp"
#include "Node.hpp"
#include "StageLayer.hpp"

EventListener::EventListener():
    node_(nullptr)
  , priority_(0)
  , state_(State::ZERO)
{}
void EventListener::reset()
{
    state_ = State::ZERO;
    priority_=0;
    node_=nullptr;
}

bool EventMouseListener::handle(Event *e)
{
    auto ev = static_cast<EventMouse*>(e);
    if(onPress(ev))
        return true;
    return false;
}
bool EventKeyboardListener::handle(Event *e)
{
    if(onPress)
        return onPress(static_cast<EventKeyboard*>(e));
    return true;
}
