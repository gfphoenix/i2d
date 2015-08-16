#include <types>
#include <debug>
#include <Scheduler.hpp>
#include <algorithm>
#include <unordered_map>

//class Scheduler;
//class SNode;
//class SHead;
//class SNode : public Ref
//{
//    protected:
//        Ref_ptr<SNode> *pprev_;
//        Ref_ptr<SNode> next_;
//        func fn_;
//        string key_;
//        bool pause_;
//        friend class Scheduler;
//        friend class SHead;
//    public:
//        SNode(const func &fn, const string &key=""):fn_(fn), key_(key), pause_(false){}
//        inline bool isPaused()const{return pause_;}
//        inline void setPaused(bool pause){pause_=pause;}
//        // return true if done, and should be removed.
//        virtual bool update(float dt){fn_(dt);return false;}
//};
//class SHead
//{
//    void *ptr_;
//    Ref_ptr<SNode> first_;
//    inline void add(SNode *sn)
//    {
//        auto last = first_;
//        sn->pprev_ = &first_;
//        sn->next_ = last;
//        first_ = sn;
//        if(last)
//            last->pprev_ = &sn->next_;
//    }
//    inline Ref_ptr<SNode> del(SNode *sn)
//    {
//        auto next = sn->next_;
//        auto pp = sn->pprev_;
//        *pp = next;
//        if(next)
//            next->pprev_ = pp;
//        return Ref_ptr<SNode>(sn);
//    }
//    inline Ref_ptr<SNode> delSafe(SNode *sn)
//    {
//        auto &&tmp = del(sn);
//        tmp->next_=nullptr;
//        tmp->pprev_ = nullptr;
//        return tmp;
//    }
//};
//class SNodeX : SNode
//{
//    float delay_;
//    float interval_;
//    float tmpDelta_;
//    float elapsed_;
//    unsigned repeat_;
//    public:
//    friend class Scheduler;
//    SNodeX(const func &fn, const string &key, float delay, float interval, unsigned repeat):
//        SNode(fn, key), delay_(delay), interval_(interval), repeat_(repeat){
//            tmpDelta_=0;elapsed_=0;
//            Assert(delay>=0.0f, "fuck !!! delay is negative");
//            Assert(interval>0.0f, "fuck !!! interval must be positive");
//        }
//    virtual bool update(float dt);
//};


class Scheduler_umap;

class SNode 
{
    protected:
        Scheduler::func fn_;
        string key_;
        friend class Scheduler_umap;
    //public:
        SNode(const Scheduler::func &fn, const string &key=""):fn_(fn), key_(key){}
        const string &getKey()const{return key_;}
        // return true if done, and should be removed.
        virtual bool update(float dt){fn_(dt);return false;}
};
class SNodeX : SNode
{
    float delay_;
    float interval_;
    float tmpDelta_;
    float elapsed_;
    unsigned repeat_;
    public:
    friend class Scheduler_umap;
    SNodeX(const Scheduler::func &fn, const string &key, float delay, float interval, unsigned repeat):
        SNode(fn, key), delay_(delay), interval_(interval), repeat_(repeat){
            tmpDelta_=0;elapsed_=0;
            Assert(delay>=0.0f, "fuck !!! delay is negative");
            Assert(interval>0.0f, "fuck !!! interval must be positive");
        }
    virtual bool update(float dt);
};
class Scheduler_umap : public Scheduler
{
    public:
        Scheduler_umap():paused_(false){}
        virtual void update(float deltaTime);
        virtual void schedulePtr(void *ptr, const string &key, const Scheduler::func &fn);
        virtual void schedulePtr(void *ptr, const string &key, const func &fn, float delay, float interval, unsigned repeat); // unimplemented yet
        virtual void unschedulePtrAll(void *ptr);
        virtual void unschedulePtrOne(void *ptr, const string &key);
        virtual void pausePtrAll(void *ptr);
        virtual void pausePtrOne(void *ptr, const string &key);
        virtual void resumePtrAll(void *ptr);
        virtual void resumePtrOne(void *ptr, const string &key);
        virtual void pause(){paused_=true;}
        virtual void resume(){paused_=false;}
        // active or inactive but not removed
        virtual bool _isScheduled(void *ptr, const string &key)const;
        // inactive but not removed
        virtual bool _isPaused(void *ptr, const string &key)const;
    private:
    public:
        std::unordered_map<void*, std::vector<SNode>> activePF_;
        std::unordered_map<void*, std::vector<SNode>> inActivePF_;
        //std::unordered_map<void*, std::vector<SNodeX>> activeInterval_;
        //std::unordered_map<void*, std::vector<SNodeX>> inActiveInterval_;
        bool paused_;
};

typedef Scheduler_umap Scheduler_impl;

bool SNodeX::update(float dt)
{
    elapsed_ += dt;
    if(elapsed_ < delay_)
        return false;
    tmpDelta_ += dt;
    if(tmpDelta_ < interval_)
        return false;
    tmpDelta_ -= interval_;
    fn_(interval_);
    if(repeat_ == 0)
        return true;
    repeat_--;

    if(tmpDelta_ >= interval_){
        // interval_ is too small
    }
    return false;
}
void Scheduler_umap::update(float deltaTime)
{
    for(auto it=activePF_.begin(),end=activePF_.end();
            it != end; it++){
        auto &vec = it->second;
        for(auto &x: vec){
            x.fn_(deltaTime);
        }
    }
}
void Scheduler_umap::schedulePtr(void *ptr, const string &key, const Scheduler::func &fn)
{
    auto it = activePF_.find(ptr);
    if(it==activePF_.end()){
        activePF_.insert(std::pair<void*,std::vector<SNode>>(ptr, std::vector<SNode>(1,SNode(fn, key))));
        return;
    }
    auto &v = it->second;
    auto iv = std::find_if(v.begin(), v.end(), [&key](const SNode &sn)->bool{return sn.key_==key;});
    Assert(iv==v.end(), "Already exists with the same key");
    v.push_back(SNode(fn, key));
}
void Scheduler_umap::schedulePtr(void *ptr, const string &key, const func &fn, float delay, float interval, unsigned repeat)
{
//    auto it = active
//    ;
}
void Scheduler_umap::unschedulePtrAll(void *ptr)
{
    auto it = activePF_.find(ptr);
    if(it!=activePF_.end())
        activePF_.erase(it);
}
void Scheduler_umap::unschedulePtrOne(void *ptr, const string &key)
{
    auto arr = {&activePF_, &inActivePF_};
    for(auto aa : arr){
        auto it = aa->find(ptr);
        if(it==aa->end())
            continue;
        auto &v = it->second;
        auto itv = std::find_if(v.begin(), v.end(), 
                [&key](const SNode &sn)->bool{return sn.getKey()==key;});
        if(itv!=v.end())
            v.erase(itv);
        if(v.size()==0)
            aa->erase(it);
    }
}
void Scheduler_umap::pausePtrAll(void *ptr)
{
    auto ita = activePF_.find(ptr);
    if(ita == activePF_.end())// no running 
        return;
    auto &data = ita->second;
    auto iti = inActivePF_.find(ptr);
    if(iti == inActivePF_.end()){// no paused, just insert it
        inActivePF_.insert({ptr,data});
    }else{
        auto &v = iti->second;
        v.insert(v.end(), data.begin(), data.end());
    }
    activePF_.erase(ita);
}
void Scheduler_umap::pausePtrOne(void *ptr, const string &key)
{
    auto ita = activePF_.find(ptr);
    if(ita == activePF_.end())// no running 
        return;
    auto &data = ita->second;
    auto it2 = std::find_if(data.begin(), data.end(), [&key](const SNode &sn)->bool{return sn.getKey()==key;});
    if(it2 == data.end())// no such key with pointer `ptr`
        return;

    auto iti = inActivePF_.find(ptr);
    if(iti == inActivePF_.end()){
        inActivePF_.insert({ptr, std::vector<SNode>(1,*it2)});
    }else{
        auto &v = iti->second;
        v.insert(v.end(), *it2);
    }

    // remove
    data.erase(it2);
    if(data.size()==0)
        activePF_.erase(ita);
}
void Scheduler_umap::resumePtrAll(void *ptr)
{
    auto itIn = inActivePF_.find(ptr);
    if(itIn==inActivePF_.end())
        return ;
    auto &data = itIn->second;
    auto ita = activePF_.find(ptr);
    if(ita == activePF_.end()){
        activePF_.insert({ptr, data});
    }else{
        auto &vv = ita->second;
        vv.insert(vv.end(), data.begin(), data.end());
    }
    inActivePF_.erase(itIn);
}
void Scheduler_umap::resumePtrOne(void *ptr, const string &key)
{
    auto itIn = inActivePF_.find(ptr);
    if(itIn == inActivePF_.end())
        return ;
    auto &tmp = itIn->second;
    auto itV = std::find_if(tmp.begin(), tmp.end(), [&key](const SNode &sn)->bool{return sn.getKey()==key;});
    if(itV==tmp.end()) // no exists
        return;
    auto ita = activePF_.find(ptr);
    if(ita == activePF_.end()){
        inActivePF_.insert({ptr, std::vector<SNode>(1, *itV)});
    }else{
        auto &vv = ita->second;
        vv.push_back(*itV);
    }
    tmp.erase(itV);

}
// active or inactive but not removed
bool Scheduler_umap::_isScheduled(void *ptr, const string &key)const
{
    auto arr = {&activePF_, &inActivePF_};
    for(auto aa : arr){
        auto it = aa->find(ptr);
        if(it != aa->end()){
            auto &data = it->second;
            auto itv = std::find_if(data.begin(), data.end(), [&key](const SNode &sn)->bool{return sn.getKey()==key;});
            if(itv!=data.end())
                return true;
        }
    }
    return false;
}
// inactive but not removed
bool Scheduler_umap::_isPaused(void *ptr, const string &key)const
{
    auto it = inActivePF_.find(ptr);
    if(it == inActivePF_.end())
        return false;
    auto &data = it->second;
    auto itv = std::find_if(data.begin(), data.end(), [&key](const SNode &sn)->bool{return sn.getKey()==key;});
    return itv!=data.end();
}
