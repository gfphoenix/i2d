#ifndef _SCHEDULE_HPP_
#define _SCHEDULE_HPP_
#include "Ref.hpp"
#include <functional>

class Ref;
class Scheduler : public Ref
{
    public:
        typedef std::function<bool(float)> bFunc;
        virtual const std::string getInfo()const{return std::string("Scheduler-0");}
        inline void schedule(Ref *node, const std::string &key, const bFunc &fn){schedule__(node, key, fn,false);}
        inline void schedule(Ref *node, const std::string &key, const bFunc &fn, float delay, float interval, unsigned long repeat)
        {schedule__(node, key, fn, delay, interval, repeat, false);}
        // key as data

        inline void unscheduleAll(Ref *node){unscheduleAll__(node);}
        inline void unscheduleOne(Ref *node, const std::string &key){unscheduleOne__(node, key);}

        inline void pauseAll(Ref *node){pauseAll__(node);}
        inline void pauseOne(Ref *node, const std::string &key){pauseOne__(node, key);}

        inline void resumeAll(Ref *node){resumeAll__(node);}
        inline void resumeOne(Ref *node, const std::string &key){resumeOne__(node, key);}

        inline bool isScheduled(Ref *node, const std::string &key="")const{return isScheduled__(node, key);}
        inline bool isPaused(Ref *node, const std::string &key="")const{return isPaused__(node, key);}

        virtual void update(float deltaTime)=0;
        inline void moveNode(Scheduler *to, Ref *node){moveNode__(to,node);}

        virtual ~Scheduler(){}
        static Ref_ptr<Scheduler> create();
    protected:
        virtual void schedule__(Ref *ptr, const std::string &key, const bFunc &fn, bool paused)=0;
        virtual void schedule__(Ref *ptr, const std::string &key, const bFunc &fn, float delay, float interval, unsigned repeat, bool paused)=0;
        virtual void unscheduleAll__(Ref *node)=0;
        virtual void unscheduleOne__(Ref *node, const std::string &key)=0;
        virtual void pauseAll__(Ref *node)=0;
        virtual void pauseOne__(Ref *node, const std::string &key)=0;
        virtual void resumeAll__(Ref *node)=0;
        virtual void resumeOne__(Ref *node, const std::string &key)=0;
        // active or inactive but not removed
        virtual bool isScheduled__(Ref *node, const std::string &key)const=0;
        // inactive but not removed
        virtual bool isPaused__(Ref *node, const std::string &key)const=0;
        virtual void pause()=0;
        virtual void resume()=0;
        virtual void moveNode__(Scheduler *to, Ref *node)=0;
        static void test();
};

#endif
