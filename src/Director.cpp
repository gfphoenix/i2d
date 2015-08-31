#include <Director.hpp>
#include <Event.hpp>
#include <Ref.hpp>
#include <Scene.hpp>
#include <mm.hpp>
#include <sys/time.h>
#include <unistd.h>

Ref_ptr<Director> Director::director_=nullptr;
Director::Director()
{
}
Director *Director::getInstance()
{
    if(director_==nullptr){
        director_ = MM<Director>::New();
    }
    return director_.get();
}
void Director::updateDeltaTime()
{
    struct timeval now;
    deltaTime_ = 0;
    if(gettimeofday(&now, nullptr) != 0 || skipOneFrame_){
        skipOneFrame_ = false;
        goto out;
    }
    deltaTime_ = now.tv_sec - lastTime_.tv_sec + (now.tv_usec - lastTime_.tv_usec)/1000000.0f;
    deltaTime_ = deltaTime_ > 0 ? deltaTime_ : 0; //std::max(0, deltaTime_)
    duration_ += deltaTime_;
out:
    lastTime_ = now;
}
void Director::onWinSizeChanged()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto &scene : sceneStack_)
        scene->onWinSizeChanged();
}
void Director::run(Scene *scene)
{
    Assert(sceneStack_.size()==0, "sceneStack must be empty");
    Assert(scene != nullptr, "Scene must not be nullptr");
    sceneStack_.push_back(scene);
    ssp_ = nullptr;
    sop_ = SceneStackOp::NOOP;
    scene->onEnter();
}
void Director::pushScene(Scene *scene)
{
    Assert(scene != nullptr, "Scene must not be nullptr");
    ssp_ = scene;
    sop_ = SceneStackOp::PUSH;
}
void Director::popScene()
{
    Assert(sceneStack_.size()>1, "popScene must be more than 2 scene");
    ssp_ = nullptr;
    sop_ = SceneStackOp::POP;
}
void Director::replaceScene(Scene *scene)
{
	Assert(sceneStack_.size()>0, "replace Scene with no scene exists");
    Assert(scene != nullptr, "Scene must not be nullptr");
    ssp_ = scene;
    sop_ = SceneStackOp::REPLACE;
}
void Director::switchScene()
{
    switch(sop_){
    case SceneStackOp::NOOP:
        return;
    case SceneStackOp::POP:{
        auto top = sceneStack_.back();
        sceneStack_.pop_back();
        top->onExit();
        if(sceneStack_.size()>0)
            sceneStack_.back()->onEnter();
        else
            end();
        break;
    }
    case SceneStackOp::REPLACE: {
        auto top = sceneStack_.back();
        sceneStack_.pop_back();
        top->onExit();
        sceneStack_.push_back(std::move(ssp_));
        sceneStack_.back()->onEnter();
        break;
    }
    case SceneStackOp::PUSH: {
        if(sceneStack_.size()>0)
            sceneStack_.back()->onExit();
        sceneStack_.push_back(std::move(ssp_));
        sceneStack_.back()->onEnter();
        break;
    }
    default:
        // panic
        break;
    }
    ssp_ = nullptr;
    sop_ = SceneStackOp::NOOP;
}
void Director::end()
{
    running_ = false;
}

void Director::mainLoop()
{
    Debug("Enter mainLoop\n");
    running_ = true;
    while(running_ && !view_->shouldClose()){
        usleep(15000);
        switchScene();
        updateDeltaTime();
        view_->pollEvents();
        onDraw();
        view_->swapBuffer();
    }
}
void Director::onDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(sceneStack_.size()>0)
        sceneStack_.back()->Render();
}
