#ifndef _SCHEDULE_HPP_
#define _SCHEDULE_HPP_
#include "Ref.hpp"
//#include "Node.hpp"
#include <functional>

class Node;
class Scheduler : public Ref
{
    public:
    //typedef std::function<void(float)> func;
    typedef std::function<bool(float)> bFunc;
    virtual const std::string getInfo()const{return std::string("Scheduler-0");}
	void scheduleUpdate(Node *node);
        inline void unscheduleUpdate(Node *node){unschedulePtrOne(static_cast<void*>(node), "");}
    inline void scheduleNode(Node *node, const std::string &key, const bFunc &fn)
        {schedulePtr(static_cast<void *>(node), key, fn);}
    //inline void scheduleNode(Node *node, const std::string &key, const bFunc &fn)
	//{schedulePtr(static_cast<void*>(node), key, fn);}
    inline void scheduleNode(Node *node, const std::string &key, const bFunc &fn, float delay, float interval, unsigned repeat)
        {schedulePtr(static_cast<void *>(node), key, fn, delay, interval, repeat);}
        // key as data

        inline void unscheduleNodeAll(Node *node){unschedulePtrAll(static_cast<void *>(node));}
        inline void unscheduleNodeOne(Node *node, const std::string &key){unschedulePtrOne(static_cast<void *>(node), key);}

        inline void pauseNodeAll(Node *node){pausePtrAll(static_cast<void *>(node));}
        inline void pauseNodeOne(Node *node, const std::string &key){pausePtrOne(static_cast<void *>(node), key);}

        inline void resumeNodeAll(Node *node){resumePtrAll(static_cast<void *>(node));}
        inline void resumeNodeOne(Node *node, const std::string &key){resumePtrOne(static_cast<void *>(node), key);}

        inline bool isScheduled(Node *node, const std::string &key)const{return isScheduled(static_cast<void *>(node), key);}
        inline bool isScheduled(Node *node)const{return _isScheduled(static_cast<void*>(node),"");}
        inline bool isScheduled(void *ptr, const std::string &key)const{return key.size()>0 && _isScheduled(ptr,key);}
        inline bool isScheduled(void *ptr){return _isScheduled(ptr,"");}
        inline bool isPaused(Node *node)const{return _isPaused(static_cast<void*>(node),"");}
        inline bool isPaused(Node *node, const std::string &key)const{return isPaused(static_cast<void*>(node),key);}
        inline bool isPaused(void *ptr, const std::string &key)const{return key.size()>0 && _isPaused(ptr,key);}


        virtual void update(float deltaTime)=0;
    virtual void schedulePtr(void *ptr, const std::string &key, const bFunc &fn)=0;
    virtual void schedulePtr(void *ptr, const std::string &key, const bFunc &fn, float delay, float interval, unsigned repeat)=0;
        virtual void unschedulePtrAll(void *ptr)=0;
        virtual void unschedulePtrOne(void *ptr, const std::string &key)=0;
        virtual void pausePtrAll(void *ptr)=0;
        virtual void pausePtrOne(void *ptr, const std::string &key)=0;
        virtual void resumePtrAll(void *ptr)=0;
        virtual void resumePtrOne(void *ptr, const std::string &key)=0;
        virtual void pause()=0;
        virtual void resume()=0;
	virtual void moveNode(Scheduler *to, Node *node)=0;
        static void test();
        static Ref_ptr<Scheduler> create();
    protected:
        virtual ~Scheduler(){}
        // active or inactive but not removed
        virtual bool _isScheduled(void *ptr, const std::string &key)const=0;
        // inactive but not removed
        virtual bool _isPaused(void *ptr, const std::string &key)const=0;
};

#endif
