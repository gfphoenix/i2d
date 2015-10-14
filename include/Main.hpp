#ifndef _MAIN_HPP
#define _MAIN_HPP

#if 0
#include <functional>
struct Hooks
{
    Hooks(){}
    Hooks(const Hooks &h):
        main(h.main),
        onEnterBackground(h.onEnterBackground),
        onEnterForeground(h.onEnterForeground){}
    Hooks(Hooks &&h):
        main(std::move(h.main)),
        onEnterBackground(std::move(h.onEnterBackground)),
        onEnterForeground(std::move(h.onEnterForeground)){}

    Hooks & operator=(const Hooks &h)
    {
        main = h.main;
        onEnterBackground = h. onEnterBackground;
        onEnterForeground = h. onEnterForeground;
        return *this;
    }
    Hooks & operator=(Hooks &&h)
    {
        main = std::move(h.main);
        onEnterBackground = std::move(h.onEnterBackground);
        onEnterForeground = std::move(h.onEnterForeground);
        return *this;
    }
    std::function<bool()> main;
    std::function<void()> onEnterBackground;
    std::function<void()> onEnterForeground;

};
#else
struct Hooks
{
    Hooks():
        main(nullptr),
        onEnterBackground(nullptr), onEnterForeground(nullptr){}

    bool (*main)();
    void (*onEnterBackground)();
    void (*onEnterForeground)();
};

#endif
#endif
