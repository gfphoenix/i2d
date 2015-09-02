#include "Action.hpp"

void ActionInterval::step(float dt)
{
    if(isFirstFrame())
	elapsed_ = 0.f;
    else
	elapsed_ += dt;
}
void ActionSet::setNode(Node *node)
{
    Action::setNode(node);
    for(auto &x : actions_)
        x->setNode(node);
}
void ActionSet::reset()
{
    for(auto &x : actions_)
        x->reset();
}
Sequence * Sequence::create(const std::initializer_list<Ref_ptr<FiniteTimeAction>>  &list)
{
    auto seq = MM<Sequence>::New();
    seq->init(list);
    seq->idx_ = 0;
    return seq;
}
Sequence * Sequence::create(std::initializer_list<Ref_ptr<FiniteTimeAction>>  &&list)
{
    auto seq = MM<Sequence>::New();
    seq->init(std::move(list));
    seq->idx_ = 0;
    return seq;
}
Sequence * Sequence::clone()const
{
    auto a = MM<Sequence>::New();
    for(auto &x : actions_){
	auto aa = x->clone();
    a->actions_.push_back(aa);
    }
    a->idx_ = 0;
    return a;
}
Sequence * Sequence::reverse()const
{
    auto a = MM<Sequence>::New();
    for(auto it=actions_.crbegin(), end=actions_.crend(); it!=end; it++){
        auto aa = (*it)->reverse();
        a->actions_.push_back(aa);
    }
    a->idx_ = 0;
    return a;
}
void Sequence::setNode(Node *node)
{
    ActionSet::setNode(node);
    return;
    FiniteTimeAction::setNode(node);
    if(actions_.size()>0)
        actions_[0]->setNode(node);
}
void Sequence::step(float dt)
{
    if(idx_>=(int)actions_.size())
        return;
    actions_[idx_]->step(dt);
}
void Sequence::update()
{
    if(idx_>=(int)actions_.size())
        return;
    auto &a = actions_[idx_];
    a->update();
    if(a->isDone() && idx_+1<(int)actions_.size()){
        actions_[++idx_]->reset();
    }
}
void Sequence::reset()
{
    idx_=0;
    ActionSet::reset();
}
Parallel * Parallel::create(std::initializer_list<Ref_ptr<FiniteTimeAction>>  &&list)
{
    auto p = MM<Parallel>::New();
    p->init(std::move(list));
    p->done_.assign(p->actions_.size(), false);
    return p;
}
Parallel * Parallel::create(const std::initializer_list<Ref_ptr<FiniteTimeAction>>  &list)
{
    auto p = MM<Parallel>::New();
    p->init(list);
    p->done_.assign(p->actions_.size(), false);
    return p;
}
Parallel * Parallel::clone()const
{
    auto a = MM<Parallel>::New();
    for(auto &x : actions_){
        auto aa = x->clone();
        a->actions_.push_back(aa);
    }
    a->done_.assign(actions_.size(), false);
    return a;
}
Parallel * Parallel::reverse()const
{
    auto a = MM<Parallel>::New();
    for(auto it=actions_.crbegin(), end=actions_.crend(); it!=end; it++){
        auto aa = (*it)->reverse();
        a->actions_.push_back(aa);
    }
    a->done_.assign(actions_.size(), false);
    return a;
}
bool Parallel::isDone()const
{
    for(auto d : done_){
    if(!d)
	    return false;
    }
    return true;
}
void Parallel::step(float dt)
{
    int i;
    int n = (int)actions_.size();
    for(i=0; i<n; i++){
        if(!done_[i])
            actions_.at(i)->step(dt);
    }
}
void Parallel::reset()
{
    ActionSet::reset();
    done_.assign(actions_.size(), false);
}

void Parallel::update()
{
    int i;
    int n = (int)actions_.size();
    for(i=0; i<n; i++){
        auto a = actions_.at(i).get();
        if(!done_[i])
            a->update();
        if(a->isDone())
            done_[i] = true;
    }
}
TimeScale * TimeScale::create(FiniteTimeAction *inner, float scale)
{
    Assert(scale>0, "time scale <=0");
    auto a = MM<TimeScale>::New();
    a->inner_ = inner;
    a->scale_ = scale;
    return a;
}
TimeScale * TimeScale::clone()const
{
    auto a = MM<TimeScale>::New();
    a->inner_ = inner_->clone();
    a->scale_ = scale_;
    return a;
}
TimeScale * TimeScale::reverse()const
{
    auto a = MM<TimeScale>::New();
    a->inner_ = inner_->reverse();
    a->scale_ = 1.f/scale_;
    return a;
}

Repeat * Repeat::create(FiniteTimeAction *inner, unsigned times)
{
    auto r = MM<Repeat>::New();
    r->init(times);
    r->inner_ = inner;
    return r;
}
Repeat * Repeat::clone()const
{
    auto r = MM<Repeat>::New();
    r->init(counter_);
    r->inner_ = inner_->clone();
    return r;
}
Repeat * Repeat::reverse()const
{
    auto r = MM<Repeat>::New();
    r->init(counter_);
    r->inner_ = inner_->reverse();
    return r;
}
void Repeat::update()
{
    inner_->update();
    if(inner_->isDone()){
        inner_->reset();
        cc_++;
    }
}
