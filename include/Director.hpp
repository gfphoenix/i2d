#ifndef _DIRECTOR_HPP
#define _DIRECTOR_HPP
#include <Ref.hpp>
#include <Event.hpp>
#include <EventDispatcher.hpp>
#include <Scene.hpp>
#include <vector>
#include <GLView.h>
#include <IdleContainer.hpp>

class TextureManager;

class Director : public Ref
{
    public:
		static Director *getInstance();
        virtual void run(const Ref_ptr<Scene> &scene)=0;
        virtual void pushScene(const Ref_ptr<Scene> &scene)=0;
        virtual void popScene()=0;
        virtual void replaceScene(const Ref_ptr<Scene> &scene)=0;
        
        virtual void end()=0;
        
        virtual void mainLoop()=0;
        virtual void onDraw()=0;
        const Ref_ptr<GLView> &getGLView()const{return view_;}
        void setGLView(const Ref_ptr<GLView> &view){view_=view;}
        virtual ~Director(){director_=nullptr;}
        inline float getDeltaTime()const{return deltaTime_;}
        inline void markNextZeroTime(){skipOneFrame_=true;}
        const IdleContainer &getIdleContainer()const{return idle_;}
    protected:
        Director();
        void updateDeltaTime();
        IdleContainer idle_;
        Ref_ptr<GLView> view_;
        Ref_ptr<EventDispatcher> eventDispatcher_;
        float deltaTime_;
        float duration_=0;
        struct timeval lastTime_;
        bool skipOneFrame_=true;
        static Ref_ptr<Director> director_;
        // all kind of resource-manager is hold by director in Ref_ptr<>
        // so they will be automatically released.
        //Ref_ptr<TextureManager> tm_;

};
class mDirector : public Director
{
    protected:
        bool running_;
        std::vector<Ref_ptr<Scene>> sceneStack_;
        Ref_ptr<Scene> next_;
        virtual ~mDirector(){}
    public:
        virtual void run(const Ref_ptr<Scene> &scene);
        //virtual void run(Ref_ptr<Scene> &&scene);
        virtual void pushScene(const Ref_ptr<Scene> &scene);
        //virtual void pushScene(Ref_ptr<Scene> &&scene);
        virtual void popScene();
        virtual void replaceScene(const Ref_ptr<Scene> &scene);
        //virtual void replaceScene(Ref_ptr<Scene> &&scene);
        virtual void end();
        virtual void mainLoop();
        virtual void onDraw();

        void handleInputKey(KeyCode code, KeyAction action, KeyMods mods);
};


#endif /* _DIRECTOR_HPP */
