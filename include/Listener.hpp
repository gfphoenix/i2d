//#ifndef _LISTENER_IMPL_HPP_
//#define _LISTENER_IMPL_HPP_
//#include <vector>
//#include <string>
//template <typename T> class Ref_ptr;
//class EventListener;
//void EventListener::Release();
//struct ListenerHelper
//{
//    std::vector<Ref_ptr<EventListener>> v_;
//    bool enable_;
//    bool hasZombie_;
//};
//
//class Listener
//{
//    private:
//        ListenerHelper *p_;
//    public:
//        Listener():p_(nullptr){}
//        ~Listener(){delete p_;}
//        inline operator bool()const{return p_!=nullptr;}
//        int size()const{return p_ ? (int)p_->v_.size() : 0;}
//        void addEventListener(Ref_ptr<EventListener> l);
//        void removeEventListener(const std::string &listenerName);
//        inline void pauseEventListener(const std::string &listenerName){setPaused(listenerName,true);}
//        inline void resumeEventListener(const std::string &listenerName){setPaused(listenerName,false);}
//        void setPaused(const std::string &listenerName, bool pause);
//        void markRemove(const std::string &listenerName);
//        bool hasZombie()const{return p_ && p_->hasZombie_;}
//        void cleanZombies();
//        void clear();
//        inline void enable()const{setEnabled(true);}
//        inline void disable()const{setEnabled(false);}
//        inline void setEnabled(bool enable)const{if(p_)p_->enable_=enable;}
//        inline bool isEnabled()const{return p_ && p_->enable_;}
//};
//
//#endif
