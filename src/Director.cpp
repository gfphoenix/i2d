#include <Director.hpp>
#include <EventDispatcher.hpp>
#include <Ref.hpp>
#include <Scene.hpp>
#include <mm.hpp>
#include <sys/time.h>

Ref_ptr<Director> Director::director_=nullptr;
Director::Director()
{
    eventDispatcher_ = MM<EventDispatcher>::New();
}
Director *Director::getInstance()
{
    if(director_==nullptr){
        director_ = MM<mDirector>::New();
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
void mDirector::run(const Ref_ptr<Scene> &scene)
{
    sceneStack_.clear();
    sceneStack_.push_back(scene);
    sceneStack_[0]->onEnter();
}
//void mDirector::run(Ref_ptr<Scene> &&scene)
//{
//    sceneStack_.clear();
//    sceneStack_.push_back(std::move(scene));
//    sceneStack_[0]->onEnter();
//}
void mDirector::pushScene(const Ref_ptr<Scene> &scene)
{
    Assert(sceneStack_.size()>0, "cannot push a scene before empty");
    sceneStack_.back()->onExit();
    sceneStack_.push_back(scene);
    sceneStack_.back()->onEnter();
}
//void mDirector::pushScene(Ref_ptr<Scene> &&scene)
//{
//    Assert(sceneStack_.size()>0, "cannot push a scene before empty");
//    sceneStack_.back()->onExit();
//    sceneStack_.push_back(std::move(scene));
//    sceneStack_.back()->onEnter();
//}
void mDirector::popScene()
{
    Assert(sceneStack_.size()>1, "popScene must be more than 2 scene");
    sceneStack_.back()->onExit();
    sceneStack_.pop_back();
    sceneStack_.back()->onEnter();
}
void mDirector::replaceScene(const Ref_ptr<Scene> &scene)
{
	Assert(sceneStack_.size()>0, "replace Scene with no scene exists");
//	auto &&top = sceneStack_.pop_back();
    auto &top = sceneStack_.back();
    sceneStack_.pop_back();
	top->onExit();
	sceneStack_.push_back(scene);
	scene->onEnter();
}
void mDirector::end()
{
    running_ = false;
}
void mDirector::mainLoop()
{
    Debug("Enter mainLoop\n");
    running_ = true;
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    while(running_ && !view_->shouldClose()){
        updateDeltaTime();
        view_->pollEvents();
        onDraw();
        //Debug("Draw ...");
        view_->swapBuffer();
    }
}
void mDirector::onDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(sceneStack_.size()>0)
        sceneStack_.back()->Render();
}
