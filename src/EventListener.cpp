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

// handle first phase: press or scroll
bool EventMouseListener::handle(Event *e)
{
    auto ev = static_cast<EventMouse*>(e);
    auto code = ev->getMouseCode();
    if(code==MouseCode::PRESS && onPress && onPress(ev))
        return true;
    if(code==MouseCode::SCROLL && onScroll && onScroll(ev))
        return true;
    return false;
}
bool EventKeyboardListener::handle(Event *e)
{
    if(onPress)
        return onPress(static_cast<EventKeyboard*>(e));
    return true;
}
