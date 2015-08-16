#ifndef _ACTION_INTERPOLATION_HPP
#define _ACTION_INTERPOLATION_HPP
#include "ActionInterval.hpp"
#include "mm.hpp"
class ActionEase : public ActionInterval
{
    public:
        static ActionEase *create(float duration, ActionInterval * inner);
        ActionEase * clone()const override=0;
        ActionEase * reverse()const override=0;
    protected:
        virtual void reset()override{ActionInterval::reset();inner_->reset();}
        virtual void step(float dt)override{ActionInterval::step(dt);inner_->step(dt);}
        virtual bool isDone()const override{return inner_->isDone();}
        virtual void setNode(Node *node)override{Action::setNode(node);inner_->setNode(node);}
        virtual void update(float percent)override{inner_->update(interpolate(percent));}
        void init(ActionInterval * inner){ActionInterval::init(inner->getDuration());inner_=inner;}
    protected:
        virtual float interpolate(float percent)const=0;
        Ref_ptr<ActionInterval> inner_;
};
typedef float (*fp)(float);
template <fp apply, fp rev=apply>
class ActionInterpolation : public ActionEase
{
    public:
        static ActionInterpolation *create(float duration, ActionInterval *inner);
        ActionInterpolation * clone()const override;
        ActionInterpolation * reverse()const override;
};
template <fp apply_, fp reverse_>
ActionInterpolation<apply_,reverse_> * ActionInterpolation<apply_,reverse_>::create(float duration, ActionInterval *inner)
{
    auto ai = MM<ActionInterpolation<apply_, reverse_>>::New();
    ai->ActionInterval::init(duration);
    ai->inner_ = inner;
    return ai;
}
template <fp apply_, fp reverse_>
ActionInterpolation<apply_,reverse_> * ActionInterpolation<apply_,reverse_>::clone()const
{
    return create(getDuration(), inner_.get());
}
template <fp apply_, fp reverse_>
ActionInterpolation<apply_,reverse_> * ActionInterpolation<apply_,reverse_>::reverse()const
{
    return ActionInterpolation<reverse_, apply_>::create(getDuration(), inner_.get());
}
#endif // _ACTION_INTERPOLATION_HPP

