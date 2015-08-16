#include <ActionInterval.hpp>
#include <mm.hpp>

RotateBy *RotateBy::create(float duration, float by)
{
    auto r = MM<RotateBy>::New();
    r->init(duration, by);
    return r;
}

RotateBy *RotateBy::clone()const
{
    return create(getDuration(), by_);
}
RotateBy *RotateBy::reverse()const
{
    return create(getDuration(), -by_);
}
void RotateBy::reset()
{
    ActionInterval::reset();
    if(node_)
        start_ = node_->getRotation();
}

void RotateBy::update(float percent)
{
    float r = start_ + by_ * percent;
    node_->setRotation(r);
}
void RotateBy::setNode(Node *node)
{
    Action::setNode(node);
    start_ = node->getRotation();
}
MoveBy * MoveBy::create(float duration, const Vec2 &dxy)
{
    auto m = MM<MoveBy>::New();
    m->init(duration, dxy);
    return m;
}
MoveBy *MoveBy::create(float duration, float dx, float dy)
{
    return create(duration, Vec2(dx,dy));
}
MoveBy *MoveBy::clone()const
{
    auto m = MM<MoveBy>::New();
    m->init(getDuration(), dXY_);
    return m;
}
MoveBy * MoveBy::reverse()const
{
    auto m = MM<MoveBy>::New();
    m->init(getDuration(), -dXY_);
    return m;
}
void MoveBy::setNode(Node *node)
{
    Action::setNode(node);
    start_ = node->getPosition();
}
void MoveBy::reset()
{
    ActionInterval::reset();
    if(node_)
        start_ = node_->getPosition();
}

void MoveBy::update(float percent)
{
    node_->setPosition(start_+dXY_*percent);
}
ScaleBy *ScaleBy::create(float duration, float dScaleX, float dScaleY)
{
    return create(duration, Vec2(dScaleX, dScaleY));
}
ScaleBy *ScaleBy::create(float duration, const Vec2 &dScale)
{
    auto s = MM<ScaleBy>::New();
    s->init(duration, dScale);
    return s;
}
ScaleBy * ScaleBy::clone()const
{
    return create(getDuration(), d2_);
}
ScaleBy * ScaleBy::reverse()const
{
    return create(getDuration(), -d2_);
}
void ScaleBy::update(float percent)
{
    node_->setScale(start_ + d2_ * percent);
}
void ScaleBy::setNode(Node *node)
{
    Action::setNode(node);
    start_ = node->getScale();
}
void ScaleBy::reset()
{
    ActionInterval::reset();
    if(node_)
        start_ = node_->getScale();
}

SizeBy * SizeBy::create(float duration, const Vec2 &dSize)
{
    auto s = MM<SizeBy>::New();
    s->init(duration, dSize);
    return s;
}
SizeBy * SizeBy::create(float duration, float dw, float dh)
{
    return create(duration, Vec2(dw,dh));
}
SizeBy * SizeBy::clone()const
{
    return create(getDuration(), dSize_);
}
SizeBy * SizeBy::reverse()const
{
    return create(getDuration(), -dSize_);
}
void SizeBy::reset()
{
    ActionInterval::reset();
    if(node_)
        oldSize_ = node_->getSize();
}
void SizeBy::setNode(Node *node)
{
    Action::setNode(node);
    oldSize_ = node->getSize();
}
void SizeBy::update(float percent)
{
    node_->setSize(oldSize_+dSize_*percent);
}

RotateTo * RotateTo::create(float duration, float to)
{
    auto r = MM<RotateTo>::New();
    r->init(duration, to);
    return r;
}
RotateTo * RotateTo::clone()const
{
    auto r = MM<RotateTo>::New();
    r->init(getDuration(), to_);
    return r;
}
void RotateTo::reset()
{
    ActionInterval::reset();
    if(node_)
        dr_ = to_ - node_->getRotation();
}
void RotateTo::update(float percent)
{
    node_->setRotation(to_-(1-percent)*dr_);
}
void RotateTo::setNode(Node *node)
{
    Action::setNode(node);
    dr_ = to_ - node->getRotation();
}
MoveTo * MoveTo::create(float duration, const Vec2 &to)
{
    auto m = MM<MoveTo>::New();
    m->init(duration, to);
    return m;
}
MoveTo * MoveTo::create(float duration, float x, float y)
{
    return create(duration, Vec2(x,y));
}
MoveTo * MoveTo::clone()const
{
    return create(getDuration(), to_);
}
void MoveTo::reset()
{
    ActionInterval::reset();
    if(node_)
        dXY_ = to_ - node_->getPosition();
}
void MoveTo::update(float percent)
{
    node_->setPosition(to_-dXY_*(1-percent));
}
void MoveTo::setNode(Node *node)
{
    Action::setNode(node);
    dXY_ = to_ - node->getPosition();
}
ScaleTo * ScaleTo::create(float duration, float sx, float sy)
{
    return create(duration, Vec2(sx, sy));
}
ScaleTo * ScaleTo::create(float duration, const Vec2 &to)
{
    auto s = MM<ScaleTo>::New();
    s->init(duration, to);
    return s;
}
ScaleTo * ScaleTo::clone()const
{
    return create(getDuration(), to_);
}
void ScaleTo::reset()
{
    ActionInterval::reset();
    if(node_)
        d2_ = to_ - node_->getScale();
}
void ScaleTo::update(float percent)
{
    node_->setScale(to_-d2_*(1-percent));
}
void ScaleTo::setNode(Node *node)
{
    Action::setNode(node);
    d2_ = to_ - node->getScale();
}
SizeTo * SizeTo::create(float duration, const Vec2 &to)
{
    auto s = MM<SizeTo>::New();
    s->init(duration, to);
    return s;
}
SizeTo * SizeTo::create(float duration, float w, float h)
{
    return create(duration, Vec2(w,h));
}
SizeTo * SizeTo::clone()const
{
    return create(getDuration(), to_);
}
void SizeTo::reset()
{
    ActionInterval::reset();
    if(node_)
        dSize_ = to_ - node_->getSize();
}
void SizeTo::update(float percent)
{
    node_->setSize(to_-dSize_*(1-percent));
}
void SizeTo::setNode(Node *node)
{
    Action::setNode(node);
    dSize_ = to_-node->getSize();
}
