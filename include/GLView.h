#ifndef GLVIEW_H_
#define GLVIEW_H_
#include "config.hpp"
#include "types.hpp"
#include "Ref.hpp"

class GLView : public Ref
{
    public:
        static GLView *create(); // FULL screen
        static GLView *create(int width, int height); // spec
        virtual ~GLView(){}
        virtual void setTitle(const char *title){(void)title;}
        virtual int getWinWidth()const=0;
        virtual int getWinHeight()const=0;
        inline Vec2 getWinSize()const{return Vec2(getWinWidth(),getWinHeight());}
        virtual bool shouldClose()const=0;
        virtual void swapBuffer()=0;
        virtual void pollEvents()=0;

        // notify director the screen size is changed
        // and the director should notify this event to all scenes.
        virtual void onResize(int width, int height)=0;
};

#endif /* GLVIEW_H_ */
