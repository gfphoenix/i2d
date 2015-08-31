#include <App.hpp>
#include <debug>
#include <Director.hpp>
#include <Main.hpp>
#include <mm.hpp>

class AppLinux : public App
{
    public:
        inline AppLinux(){};
        App::Platform getPlatform()const override{return App::Platform::Linux;}
        void go(const Hooks &hook)override;
};
void AppLinux::go(const Hooks &hooks)
{   // init director here & set the default glview
    // users can change the default glview in main
    auto d = Director::getInstance();
    auto view = GLView::create(480, 800);
    d->setGLView(view);
    this->hooks = hooks;
    auto ok = hooks.main();
    if(!ok)
        return;
    // blocking here !!!
    d->mainLoop();
}
Ref_ptr<App> App::app_ = nullptr;
App *App::getInstance()
{
    if(app_ == nullptr){
        app_ = MM<AppLinux>::New();
    }
    return app_.get();
}
