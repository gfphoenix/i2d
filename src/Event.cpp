#include "Event.hpp"

Event::Event()
//    : node_(nullptr)
//    , stopped_(false)
{
}

Touch::Touch()
    : id_(-1){}
void Touch::setTouchInfo(int id, float x, float y)
{
    previousPoint_ = currentPoint_;
    currentPoint_.x = x;
    currentPoint_.y = y;
    if(!hasInit()){
        startPoint_ = currentPoint_;
    }
    id_ = id;
}
