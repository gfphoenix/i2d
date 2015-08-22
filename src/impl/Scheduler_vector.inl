#include "types.hpp"
#include <debug>
#include <Scheduler.hpp>
#include <algorithm>
#include <functional>
#include <vector>

//namespace {
class SNode 
{
    protected:
        void *ptr_;
	Scheduler::bFunc fn_;
        std::string key_;
        bool paused_;
        bool markRemove_;
        friend class Scheduler_vector;
    public:
        SNode(void *ptr, const Scheduler::bFunc &fn, const std::string &key="")
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
        virtual void schedulePtr(void *ptr, const std::string &key, const Scheduler::bFunc &fn)override;
	//virtual void schedulePtr(void *ptr, const string &key, const Scheduler::func &fn)override;
        virtual void schedulePtr(void *ptr, const std::string &key, const Scheduler::bFunc &fn, float delay, float interval, unsigned repeat)override;
	virtual void unschedulePtrAll(void *ptr)override;
        virtual void unschedulePtrOne(void *ptr, const std::string &key)override;
	virtual void pausePtrAll(void *ptr)override;
        virtual void pausePtrOne(void *ptr, const std::string &key)override;
	virtual void resumePtrAll(void *ptr)override;
        virtual void resumePtrOne(void *ptr, const std::string &key)override;
	virtual void pause()override{paused_=true;}
	virtual void resume()override{paused_=false;}
	virtual void moveNode(Scheduler *to, Node *node)override;
        // active or inactive but not removed
        virtual bool _isScheduled(void *ptr, const std::string &key)const override;
        // inactive but not removed
        virtual bool _isPaused(void *ptr, const std::string &key)const override;
    private:
	//void schedulePtr__(void *ptr, const string &key, const Scheduler::bFunc &fn);
    public:
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
void Scheduler_vector::schedulePtr(void *ptr, const std::string &key, const Scheduler::bFunc &fn)
{
    auto tmp = std::find_if(vv_.begin(), vv_.end(),
	    [ptr,&key](const SNode &sn){return sn.ptr_==ptr && sn.key_==key;});
    Assert(tmp==vv_.end(), "Exists Scheduler with ptr=%p, and key=%s\n", ptr, key.c_str());
    vv_.push_back(SNode(ptr, fn, key));
}
struct interval_data
{
    float delay;
    float interval;
    float elapsed;
    float tmpDelta;
    //Scheduler::func fn;
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

void Scheduler_vector::schedulePtr(void *ptr, const std::string &key, const Scheduler::bFunc &fn, float delay, float interval, unsigned repeat)
{
    interval_data data(delay, interval, repeat);
    schedulePtr(ptr, key, [data,fn](float dt)mutable->bool {return data.update(dt, fn);});
}
void Scheduler_vector::unschedulePtrAll(void *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr)
	    it->markRemove_ = true;
    }
   // std::for_each(vv_.begin(), vv_.end(), [ptr](SNode &sn){if(sn.ptr_==ptr)sn.markRemove_=true;});
}
void Scheduler_vector::unschedulePtrOne(void *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr && it->key_==key)
	    it->markRemove_ = true;
    }
    //auto i1 = std::find_if(vv_.begin(), vv_.end(),
    //        [ptr,&key](SNode &sn){return sn.ptr_==ptr && sn.key_==key;});
    //if(i1 != vv_.end())
    //    i1->markRemove_ = true;
}
void Scheduler_vector::pausePtrAll(void *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr)
	    it->paused_ = true;
    }
    //std::for_each(vv_.begin(), vv_.end(), [ptr](SNode &s){if(s.ptr_==ptr)s.paused_=true;});
}
void Scheduler_vector::pausePtrOne(void *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr && it->key_ == key)
	    it->paused_ = true;
    }
   // std::for_each(vv_.begin(), vv_.end(),
   //         [ptr,&key](SNode &s){
   //         if(s.ptr_==ptr && s.key_==key)
    //        s.paused_=true;
    //        });
}
void Scheduler_vector::resumePtrAll(void *ptr)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr)
	    it->paused_ = false;
    }
   // std::for_each(vv_.begin(), vv_.end(),[ptr](SNode &s){if(s.ptr_==ptr)s.paused_=false;});
}
void Scheduler_vector::resumePtrOne(void *ptr, const std::string &key)
{
    for(auto it=vv_.begin(),end=vv_.end(); it!=end; it++){
	if(it->ptr_==ptr && it->key_==key)
	    it->paused_ = false;
    }
    //std::for_each(vv_.begin(), vv_.end(),
    //        [ptr,&key](SNode &s){
    //        if(s.ptr_==ptr && s.key_==key)
    //        s.paused_=false;
    //        });
}
void Scheduler_vector::moveNode(Scheduler *to, Node *node)
{
    for(auto it=vv_.begin(); it != vv_.end(); ){
        if(it->ptr_==(void*)node && !it->markRemove_){
            to->scheduleNode(node, it->key_, it->fn_);
            if(it->paused_)
                to->pauseNodeOne(node, it->key_);
            it = vv_.erase(it);
        }else{
            it++;
        }
    }
}

bool Scheduler_vector::_isScheduled(void *ptr, const std::string &key)const
{
    auto it1 = std::find_if(vv_.begin(), vv_.end(), 
            [ptr,&key](const SNode &s){
            return s.ptr_==ptr&&s.key_==key;
            });
    return (it1 != vv_.end()&& !it1->markRemove_);
}
bool Scheduler_vector::_isPaused(void *ptr, const std::string &key)const
{
    auto it1 = std::find_if(vv_.begin(), vv_.end(), 
            [ptr,&key](const SNode &s){
            return s.ptr_==ptr&&s.key_==key&&s.paused_;
            });
    return (it1 != vv_.end() && !it1->markRemove_);
}
//}
