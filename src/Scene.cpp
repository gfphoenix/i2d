#include <Ref.hpp>
#include <Director.hpp>
#include <Event.hpp>
#include <Scene.hpp>
#include <StageLayer.hpp>
#include <Shader.hpp>

#include <algorithm>

Scene::Scene(const Vec2 &designSize, ResolutionPolicy policy):
    StageLayer(designSize, policy),stages(nullptr)
{
    scene_ = this;
    stage_ = this;
    sch_ = Scheduler::create();
    actionManager_ = ActionManager::create();
    sch_->schedule(this, "ActionManager",
            [this](float dt)->bool{
            actionManager_->update(dt);
            return false;
            });
}
Scene::~Scene()
{
    delete stages;
}
void Scene::onWinSizeChanged()
{
    foreachStageLayer([](StageLayer *s){
            s->onWinSizeChanged();
            });
}
void Scene::addMoreStage(Ref_ptr<StageLayer> stage, int zIndex)
{
    if(stages==nullptr)
        stages = new std::vector<Ref_ptr<StageLayer>>;
    stage->scene_ = this;
    stage->setZindex(zIndex);

    stages->push_back(std::move(stage));
    if(stages->size()>1)
        updateStagePrio();
}
// FIX: no onExit running when remove
void Scene::removeExtraStageAt(int idx)
{
    if(idx<0 || stages==nullptr || idx>=(int)stages->size())
        return;
    stages->erase(stages->begin()+idx);
}
// FIX: no onExit running when remove
void Scene::removeExtraStage(StageLayer *stage)
{
    for(auto it=stages->begin(), end=stages->end(); it!=end; ++it){
        if(*it == stage){
            stages->erase(it);
            break;
        }
    }
}

void Scene::updateStagePrio()
{
    if(stages!=nullptr)
        std::stable_sort(stages->begin(), stages->end(),
                [](const Ref_ptr<StageLayer> &a, const Ref_ptr<StageLayer> &b){
                return a->getZindex()<=b->getZindex();
                });
}
void Scene::Render()
{
    glClearColor(0, 0, 0.1f, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    renderer_.startRenderCallCounter();

    int n = stages ? (int)stages->size()-1 : -1;
    while(n>=0){
        auto stage = stages->at(n).get();
        if(stage->getZindex()>=0)
            break;
        stage->Render(&renderer_);
        n--;
    }
    StageLayer::Render(&renderer_);
    while(n>=0){
        stages->at(n--)->Render(&renderer_);
    }
}
void Scene::updateListeners()
{
    int n = stages ? (int)stages->size()-1 : -1;
    while(n>=0){
        auto stage = stages->at(n).get();
        if(stage->getZindex()<0)
            break;
        stage->updateStageListeners();
        n--;
    }
    updateStageListeners();
    while(n>=0){
        stages->at(n--)->updateStageListeners();
    }
}
bool Scene::update(float dt)
{
    (void)dt;
    return true;
}

void Scene::handleSceneEvent(Event *e)
{
    // dispatch event to all stages by priority
    int n = stages ? (int)stages->size()-1 : -1;
    while(n>=0){
        auto stage = stages->at(n).get();
        if(stage->getZindex()<0)
            break;
        stage->updateStageListeners();
        if(stage->handleEvent(e))
            goto out;
        n--;
    }
    updateStageListeners();
    if(handleEvent(e))
        goto out;
    while(n>=0){
        auto stage = stages->at(n--);
        stage->updateStageListeners();
        if(stage->handleEvent(e))
            goto out;
    }
out:
    return;
}
void Scene::onEnter()
{
    foreachStageLayer([this](StageLayer *stage){
            stage->scene_ = this;
            stage->StageLayer::onEnter();
            });
}
void Scene::onExit()
{
    foreachStageLayer([this](StageLayer *stage){
            stage->StageLayer::onExit();
            });
}
void Scene::afterEnter()
{
    StageLayer::afterEnter();
    auto d = Director::getInstance();
    d->getIdleActionManager()->moveNode(getActionManager(), this);
    d->getIdleScheduler()->moveNode(getScheduler(), this);
}

