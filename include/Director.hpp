#ifndef _DIRECTOR_HPP
#define _DIRECTOR_HPP
#include "Ref.hpp"
#include "ActionManager.hpp"
#include "Scheduler.hpp"
#include "GLView.h"
#include "Scene.hpp"
#include "types.hpp"
#include <vector>

class Event;
class Director : public Ref
{
    protected:
        static Ref_ptr<Director> director_;
    public:
        static Director *getInstance();
        virtual ~Director(){director_=nullptr;}
        virtual void run(Scene *scene);
        virtual void pushScene(Scene *scene);
        virtual void popScene();
        virtual void replaceScene(Scene *scene);
        virtual void switchScene();

        inline Scene *getRunningScene()const
        {
            return sceneStack_.back().get();
        }

        virtual void end();
        inline void handleEvent(Event *e)
        {
            if(sceneStack_.size()>0)
                getRunningScene()->handleSceneEvent(e);
        }
        virtual void mainLoop();
        virtual void onDraw();
        inline const Ref_ptr<GLView> &getGLView()const{return view_;}
        inline void setGLView(const Ref_ptr<GLView> &view){view_=view;}
        inline Vec2 getWinSize()const{return view_->getWinSize();}
        virtual void onWinSizeChanged();

        inline float getDeltaTime()const{return deltaTime_;}
        inline void markNextZeroTime(){skipOneFrame_=true;}
        inline ActionManager *getIdleActionManager()const{return am_.get();}
        inline Scheduler *getIdleScheduler()const{return sched_.get();}
    protected:
        friend class MM<Director>;
        Director();
        void updateDeltaTime();

        Ref_ptr<GLView> view_;
        Ref_ptr<ActionManager> am_;
        Ref_ptr<Scheduler> sched_;
        float deltaTime_;
        float duration_=0;
        struct timeval lastTime_;
        bool skipOneFrame_=true;
        bool running_;
        // Scene manager
        std::vector<Ref_ptr<Scene>> sceneStack_;
        Ref_ptr<Scene> ssp_;
        enum class SceneStackOp
        {
            NOOP,
            PUSH,
            POP,
            REPLACE,
        };
        SceneStackOp sop_=SceneStackOp::NOOP;
};
//class mDirector : public Director
//{
//    protected:
//        bool running_;
//        std::vector<Ref_ptr<Scene>> sceneStack_;
//        Ref_ptr<Scene> next_;
//        virtual ~mDirector(){}
//    public:
//        virtual void onWinSizeChanged();
//        virtual void run(const Ref_ptr<Scene> &scene);
//        virtual void run(Ref_ptr<Scene> &&scene);
//        virtual void pushScene(const Ref_ptr<Scene> &scene);
//        virtual void pushScene(Ref_ptr<Scene> &&scene);
//        virtual void popScene();
//        virtual void replaceScene(const Ref_ptr<Scene> &scene);
//        virtual void replaceScene(Ref_ptr<Scene> &&scene);
//        virtual void end();
//        virtual void mainLoop();
//        virtual void onDraw();
//
//        void handleInputKey(KeyCode code, KeyAction action, KeyMods mods);
//};


#endif /* _DIRECTOR_HPP */
