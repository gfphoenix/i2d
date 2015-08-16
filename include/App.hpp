#ifndef _APP_HPP_
#define _APP_HPP_
#include "Ref.hpp"
#include <functional>
#include <string>
class App : public Ref
{
    protected:
        static App *app_;
        virtual ~App(){}
    public:
        static Ref_ptr<App> getInstance();
        const std::string getInfo()const{return std::string("App ");}
        enum class Platform
        {
            Unknown,
            Linux,
            Android,
            Ios,
        };
        App():Ref(){}
        bool main();
        void onEnterBackground();
        void onEnterForeground();
        virtual Platform getPlatform()const{return Platform::Unknown;}
        virtual void go()=0;
};
#endif
