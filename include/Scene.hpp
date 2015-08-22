#ifndef _SCENE_HPP
#define _SCENE_HPP

#include "Ref.hpp"
#include "Renderer.hpp"
#include <functional>
#include <string>
#include <vector>
class ActionManager;
class Scheduler;
class Shader;
class Renderer;
class StageLayer;
class Scene : public Ref
{
    protected:
    Ref_ptr<StageLayer> main_;
    std::vector<Ref_ptr<StageLayer>> *stages;
    Ref_ptr<Scheduler> sch_;
    Ref_ptr<ActionManager> actionManager_;
    Renderer renderer_;
	friend class Shader;
    friend class Director;

    public:
        Scene();
        virtual ~Scene();
        void onWinSizeChanged();
        void setMainStage(Ref_ptr<StageLayer> stage);
        void addMoreStage(Ref_ptr<StageLayer> stage, int prio);
        void updateStagePrio();
        virtual const std::string getInfo()const{return std::string("Scene: ");}
        inline Scheduler *getScheduler()const{return sch_.get();}
        inline ActionManager *getActionManager()const{return actionManager_.get();}
        // contains update, poll-event and render
        void mainLoop();
        virtual void Render();
        virtual void pollEvent();
        virtual void update(float dt);

        virtual void onEnter();
        virtual void onExit();

    protected:
        void foreachStageLayer(const std::function<void(StageLayer *stage)> &fn);
};
#endif
