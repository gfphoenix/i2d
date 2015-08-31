#ifndef _LISTENER_VECTOR_HPP_
#define _LISTENER_VECTOR_HPP_
#include "EventListener.hpp"

class Node;
struct ListenerVector
{
    std::vector<Ref_ptr<EventListener>> *v_;
    public:
    ListenerVector():v_(nullptr){}
    ~ListenerVector(){
        clear();
    }

    std::vector<Ref_ptr<EventListener>> &getEventListeners(){return *v_;}
    // return true if need to update listener dispatcher
    inline bool addListener(EventListener *l)
    {
        if(l->isRegistered())
            return false;
        if(v_==nullptr)
            v_ = new std::vector<Ref_ptr<EventListener>>;
        v_->push_back(l);
        return true;
    }
    bool removeListener(EventListener *l)
    {
        bool ok = false;
        if(v_==nullptr)
            goto out ;
        for(auto x=v_->begin(), end=v_->end(); x!=end; ++x)
            if((*x)==l){
                (*x)->setDead();
                v_->erase(x);
                ok = true;
                break;
            }
        if(v_->size()==0){
            delete v_;
            v_=nullptr;
        }
    out:
        return ok;
    }
    void clear()
    {
        if(!v_)
            return;
        int i=(int)v_->size();
        auto &v = *v_;
        while(i-->0){
            auto l = v.at(i).get();
            l->setDead();
            l->setNode(nullptr);
        }
        delete v_;
        v_=nullptr;
    }
    friend class Node;
};

#endif
