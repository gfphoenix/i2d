#ifndef _APP_HPP_
#define _APP_HPP_
#include "Main.hpp"
#include "Ref.hpp"
class App : public Ref
{
    protected:
        static Ref_ptr<App> app_;
        Hooks hooks;
        virtual ~App(){}
    public:
        static App *getInstance();
        enum class Platform
        {
            Unknown,
            Linux,
            Android,
            Ios,
        };
        App():Ref(){}

        virtual void onEnterBackground();
        virtual void onEnterForeground();
        virtual Platform getPlatform()const{return Platform::Unknown;}
        virtual void go(const Hooks &hooks)=0;
};
#endif
