#include <App.hpp>
#include <debug>
#include <Director.hpp>
#include <mm.hpp>

class AppLinux : public App
{
    public:
        App::Platform getPlatform()const override;
        void go()override;
};
App::Platform AppLinux::getPlatform()const
{
    return App::Platform::Linux;
}
void AppLinux::go()
{
    auto d = Director::getInstance();
    auto view = d->getGLView();
    if(view == nullptr){
        view = GLView::create();
        d->setGLView(view);
    }
    auto ok = this->main();
    if(!ok)
        return;
    //realy Go
    d->mainLoop();

    {
        delete d;
    }
}
App *App::app_ = nullptr;
Ref_ptr<App> App::getInstance()
{
    if(app_ == nullptr){
        app_ = MM<AppLinux>::New();
    }
    return Ref_ptr<App>(app_);
}
