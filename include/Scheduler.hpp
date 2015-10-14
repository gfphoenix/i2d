#ifndef _SCHEDULE_HPP_
#define _SCHEDULE_HPP_
#include "Ref.hpp"
#include <functional>

class Scene;
class Node;
class Scheduler : public Ref
{
    public:
        typedef std::function<bool(float)> bFunc;
        virtual const std::string getInfo()const{return std::string("Scheduler-0");}
        inline void schedule(Node *node, const std::string &key, const bFunc &fn){schedule__(node, key, fn,false);}
        inline void schedule(Node *node, const std::string &key, const bFunc &fn, float delay, float interval, unsigned long repeat)
        {schedule__(node, key, fn, delay, interval, repeat, false);}
        // key as data

        inline void unscheduleAll(Node *node){unscheduleAll__(node);}
        inline void unscheduleOne(Node *node, const std::string &key){unscheduleOne__(node, key);}

        inline void pauseAll(Node *node){pauseAll__(node);}
        inline void pauseOne(Node *node, const std::string &key){pauseOne__(node, key);}

        inline void resumeAll(Node *node){resumeAll__(node);}
        inline void resumeOne(Node *node, const std::string &key){resumeOne__(node, key);}

        inline bool isScheduled(Node *node, const std::string &key="")const{return isScheduled__(node, key);}
        inline bool isPaused(Node *node, const std::string &key="")const{return isPaused__(node, key);}

        virtual void update(float deltaTime)=0;
        inline void moveNode(Scheduler *to, Scene *scene){moveNode__(to,scene);}

        virtual ~Scheduler(){}
        static Ref_ptr<Scheduler> create();
    protected:
        friend class Scene;
        virtual void schedule__(Node *ptr, const std::string &key, const bFunc &fn, bool paused)=0;
        virtual void schedule__(Node *ptr, const std::string &key, const bFunc &fn, float delay, float interval, unsigned repeat, bool paused)=0;
        virtual void unscheduleAll__(Node *node)=0;
        virtual void unscheduleOne__(Node *node, const std::string &key)=0;
        virtual void pauseAll__(Node *node)=0;
        virtual void pauseOne__(Node *node, const std::string &key)=0;
        virtual void resumeAll__(Node *node)=0;
        virtual void resumeOne__(Node *node, const std::string &key)=0;
        // active or inactive but not removed
        virtual bool isScheduled__(Node *node, const std::string &key)const=0;
        // inactive but not removed
        virtual bool isPaused__(Node *node, const std::string &key)const=0;
        virtual void pause()=0;
        virtual void resume()=0;
        virtual void moveNode__(Scheduler *to, Scene *node)=0;
        static void test();
};

#endif
