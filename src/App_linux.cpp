#include <App.hpp>
#include <debug>
#include <Director.hpp>
#include <Log.hpp>
#include <Main.hpp>
#include <mm.hpp>
#include <stdio.h>
class LoggerFile : public Logger
{
public:
//    virtual Cerror write(const char *fmt, ...)override;
    virtual Cerror vwrite(const char *fmt, va_list ap)override;
    LoggerFile(const char *filename);
    virtual ~LoggerFile();
protected:
    FILE *file_;
};
LoggerFile::LoggerFile(const char *filename)
{
    file_ = fopen(filename, "w+");
}
LoggerFile::~LoggerFile()
{
    if(file_)
        fclose(file_);
}

Cerror LoggerFile::vwrite(const char *fmt, va_list ap)
{
    vfprintf(file_, fmt, ap);
    return Cerror(nullptr);
}

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
    Log::setLogger(MM<LoggerFile>::New("log.txt"));
    Log::i("start to GO...!");
    auto d = Director::getInstance();
    auto view = GLView::create(360, 600);
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
