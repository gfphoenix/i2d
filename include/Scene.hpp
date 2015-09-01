#ifndef _SCENE_HPP
#define _SCENE_HPP

#include "Ref.hpp"
#include "Renderer.hpp"
#include "StageLayer.hpp"
#include <functional>
#include <string>
#include <vector>
class ActionManager;
class Scheduler;
class Shader;
class Renderer;
class StageLayer;
class Scene : public StageLayer
{
    protected:
        std::vector<Ref_ptr<StageLayer>> *stages;
        Ref_ptr<Scheduler> sch_;
        Ref_ptr<ActionManager> actionManager_;
        Renderer renderer_;
        friend class Shader;
        friend class Director;

    public:
        Scene(const Vec2 &designSize, ResolutionPolicy policy=ResolutionPolicy::KEEP);
        virtual ~Scene();
        void onWinSizeChanged();
        void addMoreStage(Ref_ptr<StageLayer> stage, int zIndex=0);
        inline int extraStages()const{return stages==nullptr ? 0 : stages->size();}
        inline StageLayer *getStageAt(int idx)const{return stages->at(idx).get();}
        void removeExtraStageAt(int idx);
        void removeExtraStage(StageLayer *stage);
        void updateStagePrio();
        // when add or remove node that affect the listener order
        virtual void updateListeners();

        inline Scheduler *getScheduler()const{return sch_.get();}
        inline ActionManager *getActionManager()const{return actionManager_.get();}
        virtual void Render();
        virtual bool update(float dt)override;

        virtual void onEnter();
        virtual void onExit();
        virtual void handleSceneEvent(Event *e);
    protected:
        //
        inline void foreachStageLayer(void (*fn)(StageLayer *stage, void *userData), void *userData)
        {
            int i=0;
            if(stages!=nullptr)
                for(; i<(int)(stages->size()); ++i){
                    auto s = stages->at(i).get();
                    if(s->getZindex()>=0)
                        break;
                    fn(s, userData);
                }
            fn(this, userData);
            if(stages!=nullptr)
                for(; i<(int)(stages->size()); i++){
                    auto s = stages->at(i).get();
                    fn(s, userData);
                }
        }
        inline void foreachStageLayerReverse(void (*fn)(StageLayer *stage, void *userData), void *userData)
        {
            int i=stages!=nullptr ? (int)(stages->size())-1 : -1;
            while(i>=0){
                auto s = stages->at(i).get();
                if(s->getZindex()<0)
                    break;
                fn(s, userData);
                --i;
            }
            fn(this, userData);
            while(i>=0){
                auto s = stages->at(i).get();
                fn(s, userData);
                --i;
            }
        }

        inline void foreachStageLayer(const std::function<void(StageLayer *stage)> &fn)
        {
            int i=0;
            if(stages!=nullptr)
                for(; i<(int)(stages->size()); ++i){
                    auto s = stages->at(i).get();
                    if(s->getZindex()>=0)
                        break;
                    fn(s);
                }
            fn(this);
            if(stages!=nullptr)
                for(; i<(int)(stages->size()); i++){
                    auto s = stages->at(i).get();
                    fn(s);
                }
        }
        inline void foreachStageLayerReverse(const std::function<void(StageLayer*)> &fn)
        {
            int i=stages!=nullptr ? (int)(stages->size())-1 : -1;
            while(i>=0){
                auto s = stages->at(i).get();
                if(s->getZindex()<0)
                    break;
                fn(s);
                --i;
            }
            fn(this);
            while(i>=0){
                auto s = stages->at(i).get();
                fn(s);
                --i;
            }
        }
};
#endif
