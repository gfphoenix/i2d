#include <App.hpp>
void App::onEnterBackground()
{
    if(hooks.onEnterBackground)
        hooks.onEnterBackground();
}
void App::onEnterForeground()
{
    if(hooks.onEnterForeground)
        hooks.onEnterForeground();
}
