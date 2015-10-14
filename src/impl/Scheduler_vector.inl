#include "types.hpp"
#include <debug>
#include <Scheduler.hpp>
#include <Node.hpp>
#include <algorithm>
#include <functional>
#include <vector>

class SNode 
{
    public:
        Node *ptr_;
        Scheduler::bFunc fn_;
        std::string key_;
        bool paused_;
        bool markRemove_;
        friend class Scheduler_vector;

        SNode(Node *ptr, const Scheduler::bFunc &fn, const std::string &key="")
            :ptr_(ptr), fn_(fn), key_(key), paused_(false),markRemove_(false){}
        SNode(const SNode &s)
            :ptr_(s.ptr_),fn_(s.fn_), key_(s.key_)
             , paused_(s.paused_), markRemove_(s.markRemove_){}
        SNode(SNode &&s)
            :ptr_(s.ptr_), fn_(std::move(s.fn_)), key_(std::move(s.key_))
             , paused_(s.paused_), markRemove_(s.markRemove_){}
        SNode &operator=(const SNode &s){
            ptr_ = s.ptr_;
            fn_ = s.fn_;
            key_ = s.key_;
            paused_ = s.paused_;
            markRemove_ = s.markRemove_;
            return *this;
        }
        SNode &operator=(SNode &&s){
            ptr_ = s.ptr_;
            fn_ = std::move(s.fn_);
            key_ = std::move(s.key_);
            paused_ = s.paused_;
            markRemove_ = s.markRemove_;
            return *this;
        }
        const std::string &getKey()const{return key_;}
        // return true if done, and should be removed.
        inline bool update(float dt)const{return fn_(dt);}
};

class Scheduler_vector : public Scheduler
{
    public:
        Scheduler_vector():paused_(false){}

        virtual void update(float deltaTime)override;
        virtual void schedule__(Node *ptr, const std::string &key, const Scheduler::bFunc &fn, bool paused)override;
        virtual void schedule__(Node *ptr, const std::string &key, const Scheduler::bFunc &fn, float delay, float interval, unsigned repeat, bool paused)override;
        virtual void unscheduleAll__(Node *ptr)override;
        virtual void unscheduleOne__(Node *ptr, const std::string &key)override;
        virtual void pauseAll__(Node *ptr)override;
        virtual void pauseOne__(Node *ptr, const std::string &key)override;
        virtual void resumeAll__(Node *ptr)override;
        virtual void resumeOne__(Node *ptr, const std::string &key)override;
        virtual void pause()override{paused_=true;}
        virtual void resume()override{paused_=false;}
        virtual void moveNode__(Scheduler *to, Scene *scene)override;
        // active or inactive but not removed
        virtual bool isScheduled__(Node *ptr, const std::string &key)const override;
        // inactive but not removed
        virtual bool isPaused__(Node *ptr, const std::string &key)const override;
    public:
    protected:
        std::vector<SNode> vv_;
        bool paused_;
};

typedef Scheduler_vector Scheduler_impl;

void Scheduler_vector::update(float deltaTime)
{
    if(paused_)
        return;
    for(auto it=vv_.begin(); it!=vv_.end(); ){
        if(it->markRemove_ || (!it->paused_ && it->update(deltaTime))){
            it = vv_.erase(it);
        }else{
            it++;
        }
    }
}
void Scheduler_vector::schedule__(Node *ptr, const std::string &key, const Scheduler::bFunc &fn, bool paused)
{
    auto tmp = std::find_if(vv_.begin(), vv_.end(),
            [ptr,&key](const SNode &sn){return sn.ptr_==ptr && sn.key_==key;});
    Assert(tmp==vv_.end(), "Exists Scheduler with ptr=%p, and key=%s\n", ptr, key.c_str());
    SNode x(ptr, fn, key);
    x.paused_ = paused;
    vv_.push_back(std::move(x));
}
struct interval_data
{
    float delay;
    float interval;
    float elapsed;
    float tmpDelta;
    unsigned repeat;

    interval_data(float delay, float interval, unsigned repeat)
        :delay(delay), interval(interval)
         , elapsed(0), tmpDelta(0),
         repeat(repeat)
    {}
    bool update(float da, const Scheduler::bFunc &fn);
};
bool interval_data::update(float dt, const Scheduler::bFunc &fn)
{
    elapsed += dt;
    if(elapsed < delay)
        return false;
    tmpDelta += dt;
    if(tmpDelta < interval)
        return false;
    tmpDelta -= interval;
    auto stop = fn(interval);
    if(stop || repeat == 0)
        return true;
    repeat--;
    if(tmpDelta >= interval){
        // interval_ is too small
    }
    return false;
}

void Scheduler_vector::schedule__(Node *ptr, const std::string &key, const Scheduler::bFunc &fn, float delay, float interval, unsigned repeat, bool paused)
{
    interval_data data(delay, interval, repeat);
    schedule__(ptr, key, [data,fn](float dt)mutable->bool{return data.update(dt, fn);}, paused);
}
void Scheduler_vector::unscheduleAll__(Node *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr)
            it->markRemove_ = true;
    }
}
void Scheduler_vector::unscheduleOne__(Node *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr && it->key_==key){
            it->markRemove_ = true;
            break;
        }
    }
}
void Scheduler_vector::pauseAll__(Node *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr)
            it->paused_ = true;
    }
}
void Scheduler_vector::pauseOne__(Node *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr && it->key_ == key){
            it->paused_ = true;
            break;
        }
    }
}
void Scheduler_vector::resumeAll__(Node *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr)
            it->paused_ = false;
    }
}
void Scheduler_vector::resumeOne__(Node *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr && it->key_==key){
            it->paused_ = false;
            break;
        }
    }
}
void Scheduler_vector::moveNode__(Scheduler *to, Scene *scene)
{
    auto to_ = static_cast<Scheduler_vector*>(to);
    for(auto it=vv_.begin(); it != vv_.end(); ){
        auto node = it->ptr_;
        if(node->getScene()==scene && !it->markRemove_){
            to_->schedule__(node, it->key_, it->fn_, it->paused_);
            it = vv_.erase(it);
        }else{
            it++;
        }
    }
}
bool Scheduler_vector::isScheduled__(Node *ptr, const std::string &key)const
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr && it->key_==key)
            return !it->markRemove_;
    }
    return false;
}
bool Scheduler_vector::isPaused__(Node *ptr, const std::string &key)const
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
        if(it->ptr_==ptr && it->key_==key)
            return !it->markRemove_ && it->paused_;
    }
    return false;
}
