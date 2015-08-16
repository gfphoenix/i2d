#ifndef _LISTENER_VECTOR_HPP_
#define _LISTENER_VECTOR_HPP_
#include "EventListener.hpp"

class Node;
struct ListenerVector
{
    std::vector<Ref_ptr<EventListener>> v_;
    bool enable_;
    bool hasZombies_;
    public:
    ListenerVector()
        : enable_(true)
          , hasZombies_(false)
    {}
    inline bool hasZombies()const{return hasZombies_;}
    // enable or disable all listeners
    inline bool isEnabled()const{return enable_;}
    inline void enable(){enable_=true;}
    inline void disable(){enable_=false;}
    inline void setEnabled(bool enable){enable_=enable;}
    std::vector<Ref_ptr<EventListener>> &getEventListeners(){return v_;}
    inline void addListener(Ref_ptr<EventListener> &&l)
    {
        v_.push_back(std::move(l));
    }
    inline void removeListener(Ref_ptr<EventListener> &&l)
    {
        hasZombies_ = true;
        l->markRemove();
    }
    inline void cleanZombies()
    {
        int i,j;
        int n = (int)v_.size();
        auto data = v_.data();
        i=j=0;
        for(; j<n; j++)
            if(!data[j]->shouldRemove()){
                if(i!=j)
                    data[i] = std::move(data[j]);
                i++;
            }
        v_.resize(i);
    }
    friend class Node;
};

#endif
