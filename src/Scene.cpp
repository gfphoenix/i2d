#include <Ref.hpp>
#include <Director.hpp>
#include <Scene.hpp>
#include <StageLayer.hpp>
#include <Shader.hpp>

#include <algorithm>

Scene::Scene():
    stages(nullptr)
{
    sch_ = Scheduler::create();
    actionManager_ = ActionManager::create();
    sch_->schedulePtr(actionManager_.get(), "ActionManager", [this](float dt){actionManager_->update(dt);return false;});
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
    //main_->onWinSizeChanged();
    //if(stages!=nullptr)
    //for(auto &s : *stages)
    //    s->onWinSizeChanged();
}
void Scene::setMainStage(Ref_ptr<StageLayer> stage)
{
    stage->scene_ = this;
    main_ = std::move(stage);
}
void Scene::addMoreStage(Ref_ptr<StageLayer> stage, int prio)
{
    if(stages==nullptr)
        stages = new std::vector<Ref_ptr<StageLayer>>;
    stage->scene_ = this;
    stage->setPrio(prio);

    stages->push_back(std::move(stage));
    if(stages->size()>1)
        updateStagePrio();
}
void Scene::updateStagePrio()
{
    if(stages!=nullptr)
        std::stable_sort(stages->begin(), stages->end(),
        [](const Ref_ptr<StageLayer> &a, const Ref_ptr<StageLayer> &b){
            return a->getPrio()<=b->getPrio();
        });
}

void Scene::Render()
{

    int i;
    glClearColor(0, 0, 0.1f, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    renderer_.startRenderCallCounter();
    if(stages!=nullptr)
        for(i=0; i<(int)stages->size(); i++){
            auto s = stages->at(i).get();
            if(s->getPrio()>=0)
                break;
            s->Render(&renderer_);
        }
    main_->Render(&renderer_);
    if(stages!=nullptr)
        for(; i<(int)stages->size(); i++){
            auto s = stages->at(i).get();
            s->Render(&renderer_);
        }
}

// schedule update, action runnning, and drawing
void Scene::mainLoop()
{
   // auto d = Director::getInstance();

}
void Scene::update(float dt)
{
(void)dt;
}
void Scene::pollEvent()
{

}

void Scene::onEnter()
{
    foreachStageLayer([this](StageLayer *stage){
            stage->scene_ = this;
            stage->onEnter();
            });
}
void Scene::onExit()
{

}
void Scene::foreachStageLayer(const std::function<void(StageLayer *stage)> &fn)
{
    auto p = main_.get();
    if(p != nullptr)
        fn(p);
    if(stages != nullptr)
    for(auto &s : *stages)
        fn(s.get());
}
