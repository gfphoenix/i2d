#include <Ref.hpp>
#include <Director.hpp>
#include <Scene.hpp>
#include <Shader.hpp>

#include <algorithm>

Scene::Scene():
    stages(nullptr)
{
    sch_ = Scheduler::create();
    actionManager_ = ActionManager::create();
    sch_->schedulePtr(actionManager_.get(), "ActionManager", [this](float dt){actionManager_->update(dt);return false;});
}
void Scene::setMainStage(Ref_ptr<StageLayer> stage)
{
    main_ = std::move(stage);
}
void Scene::addMoreStage(Ref_ptr<StageLayer> stage, int prio)
{
    if(stages==nullptr)
        stages = new std::vector<Ref_ptr<StageLayer>>;
    stage->setPrio(prio);
    stages->push_back(std::move(stage));
    if(stages->size()>1)
        updateStagePrio();
}
void Scene::updateStagePrio()
{
    if(stages!=null)
        std::stable_sort(stages->begin(), stages->end(),
        [](const Ref_ptr<StageLayer> &a, const Ref_ptr<StageLayer> &b){
            return a->getPrio()<=b->getPrio();
        });
}

void Scene::Render()
{
    sch_->update(Director::getInstance()->getDeltaTime());
//    glViewport(0,0,320,480);
    renderCallCounter_ = 0;
    this->visit(this, Mat3(), false);
    Flush();
   // Debug("Render Call = %d\n", renderCalls_);
}
void Scene::Render()
{
    int i;
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(stages!=null)
        for(i=0; i<stages->size(); i++){
            auto s = stages->at(i).get();
            if(s->getPrio()>=0)
                break;
            s->Render(&renderer_);
            s->Flush(&renderer_);
        }
    main_->Render(&renderer_);
    main_->Flush(&renderer_);
    if(stages!=null)
        for(; i<stages->size(); i++){
            auto s = stages->at(i).get();
            s->Render(&renderer_);
            s->Flush(&renderer_);
        }
}

// schedule update, action runnning, and drawing
void Scene::mainLoop()
{
    auto d = Director::getInstance();

}
