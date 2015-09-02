#include <Ref.hpp>
#include <Director.hpp>
#include <Event.hpp>
#include <Scene.hpp>
#include <StageLayer.hpp>
#include <Shader.hpp>

#include <algorithm>

Scene::Scene(const Vec2 &designSize, ResolutionPolicy policy):
    StageLayer(designSize, policy),stages_(nullptr)
{
    scene_ = this;
    stage_ = this;
    scheduler_ = Scheduler::create();
    actionManager_ = ActionManager::create();
    scheduler_->schedule(this, "ActionManager",
            [this](float dt)->bool{
            actionManager_->update(dt);
            return false;
            });
}
Scene::~Scene()
{
    delete stages_;
}
void Scene::onWinSizeChanged()
{
    foreachStageLayer([](StageLayer *s){
            s->onWinSizeChanged();
            });
}
void Scene::addExtraStage(Ref_ptr<StageLayer> stage, int zIndex)
{
    if(stages_==nullptr)
        stages_ = new std::vector<Ref_ptr<StageLayer>>;
    stage->scene_ = this;
    stage->setZindex(zIndex);

    stages_->push_back(std::move(stage));
    if(stages_->size()>1)
        updateStagePrio();
}
// FIX: no onExit running when remove
void Scene::removeExtraStageAt(int idx)
{
    if(idx<0 || stages_==nullptr || idx>=(int)stages_->size())
        return;
    stages_->erase(stages_->begin()+idx);
}
// FIX: no onExit running when remove
void Scene::removeExtraStage(StageLayer *stage)
{
    for(auto it=stages_->begin(), end=stages_->end(); it!=end; ++it){
        if(*it == stage){
            stages_->erase(it);
            break;
        }
    }
}

void Scene::updateStagePrio()
{
    if(stages_!=nullptr)
        std::stable_sort(stages_->begin(), stages_->end(),
                [](const Ref_ptr<StageLayer> &a, const Ref_ptr<StageLayer> &b){
                return a->getZindex()<=b->getZindex();
                });
}
void Scene::Render()
{
    glClearColor(0, 0, 0.1f, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    renderer_.startRenderCallCounter();

    int n = stages_ ? (int)stages_->size()-1 : -1;
    while(n>=0){
        auto stage = stages_->at(n).get();
        if(stage->getZindex()>=0)
            break;
        stage->Render(&renderer_);
        n--;
    }
    StageLayer::Render(&renderer_);
    while(n>=0){
        stages_->at(n--)->Render(&renderer_);
    }
}
void Scene::updateListeners()
{
    int n = stages_ ? (int)stages_->size()-1 : -1;
    while(n>=0){
        auto stage = stages_->at(n).get();
        if(stage->getZindex()<0)
            break;
        stage->updateStageListeners();
        n--;
    }
    updateStageListeners();
    while(n>=0){
        stages_->at(n--)->updateStageListeners();
    }
}
bool Scene::update(float dt)
{
    (void)dt;
    return true;
}

void Scene::handleSceneEvent(Event *e)
{
    // dispatch event to all stages_ by priority
    int n = stages_ ? (int)stages_->size()-1 : -1;
    while(n>=0){
        auto stage = stages_->at(n).get();
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
        auto stage = stages_->at(n--);
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

