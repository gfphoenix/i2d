#include "EventListener.hpp"

EventListener::EventListener()
{
    reset();
}
void EventListener::init(const std::function<void (Event *)> &fn)
{
    fn_ = fn;
    //paused_ = false;
}
void EventListener::reset()
{
//    paused_=false;
    //registered_=false;
    state_ = State::ZERO;
    priority_=0;
    node_=nullptr;
    fn_ = nullptr;
}
