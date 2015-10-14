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
        std::vector<Ref_ptr<StageLayer>> *stages_;
        Ref_ptr<Scheduler> scheduler_;
        Ref_ptr<ActionManager> actionManager_;
        Renderer renderer_;
        friend class Shader;
        friend class Director;

    public:
        Scene(const Vec2 &designSize, ResolutionPolicy policy=ResolutionPolicy::KEEP);
        virtual ~Scene();
        void onWinSizeChanged();
        void addExtraStage(Ref_ptr<StageLayer> stage, int zIndex=0);
        inline int getExtraStageNumber()const{return stages_==nullptr ? 0 : (int)stages_->size();}
        inline StageLayer *getStageAt(int idx)const{return stages_->at(idx).get();}
        void removeExtraStageAt(int idx);
        void removeExtraStage(StageLayer *stage);

        inline Scheduler *getScheduler()const{return scheduler_.get();}
        inline ActionManager *getActionManager()const{return actionManager_.get();}
        virtual void Render();
        virtual bool update(float dt)override;

        virtual void onEnter()override;
        virtual void onExit()override;
        virtual void afterEnter()override;
        virtual void handleSceneEvent(Event *e);
    protected:
        // when add or remove node that affect the listener order
        virtual void updateListeners();
        void updateStagePrio();
        //
        inline void foreachStageLayer(void (*fn)(StageLayer *stage, void *userData), void *userData)
        {
            int i=0;
            if(stages_!=nullptr)
                for(; i<(int)(stages_->size()); ++i){
                    auto s = stages_->at(i).get();
                    if(s->getZindex()>=0)
                        break;
                    fn(s, userData);
                }
            fn(this, userData);
            if(stages_!=nullptr)
                for(; i<(int)(stages_->size()); i++){
                    auto s = stages_->at(i).get();
                    fn(s, userData);
                }
        }
        inline void foreachStageLayerReverse(void (*fn)(StageLayer *stage, void *userData), void *userData)
        {
            int i=stages_!=nullptr ? (int)(stages_->size())-1 : -1;
            while(i>=0){
                auto s = stages_->at(i).get();
                if(s->getZindex()<0)
                    break;
                fn(s, userData);
                --i;
            }
            fn(this, userData);
            while(i>=0){
                auto s = stages_->at(i).get();
                fn(s, userData);
                --i;
            }
        }

        inline void foreachStageLayer(const std::function<void(StageLayer *stage)> &fn)
        {
            int i=0;
            if(stages_!=nullptr)
                for(; i<(int)(stages_->size()); ++i){
                    auto s = stages_->at(i).get();
                    if(s->getZindex()>=0)
                        break;
                    fn(s);
                }
            fn(this);
            if(stages_!=nullptr)
                for(; i<(int)(stages_->size()); i++){
                    auto s = stages_->at(i).get();
                    fn(s);
                }
        }
        inline void foreachStageLayerReverse(const std::function<void(StageLayer*)> &fn)
        {
            int i=stages_!=nullptr ? (int)(stages_->size())-1 : -1;
            while(i>=0){
                auto s = stages_->at(i).get();
                if(s->getZindex()<0)
                    break;
                fn(s);
                --i;
            }
            fn(this);
            while(i>=0){
                auto s = stages_->at(i).get();
                fn(s);
                --i;
            }
        }
};
#endif
