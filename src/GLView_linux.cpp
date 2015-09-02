#include "GLView.h"
#include "input/input_linux.hpp"
#include "Event.hpp"
//#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"
#include <mm.hpp>
#include <Director.hpp>

//
/** mouse event:
left:
    press, move, release
right:
    press, release
middle:
    press, scroll, release
*/

class GLViewLinux : public GLView
{
public:
        GLFWwindow *win_;
        int width_;
        int height_;
        Vec2 cursor_; // mouse pos
        Vec2 cursorStart_; // cursor pos when mouse button pressed
        bool pressedLeft; // to track the move action

        static GLViewLinux *createGLViewLinux(int w, int h, const char *title);
        static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods);
        static void scroll_callback(GLFWwindow *win, double xoffset, double yoffset);
        static void mouse_button_callback(GLFWwindow *win, int button, int action, int mods);
        static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        static void winsize_callback(GLFWwindow *win, int width, int height);

        virtual void setTitle(const char *title)override{::glfwSetWindowTitle(win_, title);}
        virtual int getWinWidth()const override{return width_;}
        virtual int getWinHeight()const override{return height_;}
        virtual bool shouldClose()const override{return ::glfwWindowShouldClose(win_);}
        virtual void swapBuffer()override{::glfwSwapBuffers(win_);}
        virtual void pollEvents()override{::glfwPollEvents();}
        inline GLViewLinux():GLView(){}
        friend class MM<GLViewLinux>;
        friend void winsize_callback(GLFWwindow *win, int width, int height);
    public:
        Vec2 getCursor(){
            double x,y;
            ::glfwGetCursorPos(win_, &x, &y);
            return Vec2((float)x, height_-(float)y);
        }

        const Vec2 &updateStartCursor()
        {
            cursorStart_ = getCursor();
            return cursorStart_;
        }

        inline virtual void onResize(int w, int h)override{setSize(w,h);}
        inline void setSize(int w, int h){width_=w;height_=h;}
        inline void setWin(GLFWwindow *win){win_ = win;}
        virtual ~GLViewLinux(){::glfwDestroyWindow(win_);}
};
inline static KeyMods toKeyMods(int mods)
{
    unsigned m = 0;
    if(mods & GLFW_MOD_ALT)
        m |= MOD_ALT;
    if(mods & GLFW_MOD_CONTROL)
        m |= MOD_CTRL;
    if(mods & GLFW_MOD_SHIFT)
        m |= MOD_SHIFT;
    if(mods & GLFW_MOD_SUPER)
        m |= MOD_SUPER;
    return (KeyMods)m;
}

GLViewLinux *GLViewLinux::createGLViewLinux(int w, int h, const char *title)
{
    ::glfwInit();
    ::glfwWindowHint(GLFW_SAMPLES, 4);

    //::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    auto win = ::glfwCreateWindow(w, h, title, nullptr, nullptr);
    if(win == nullptr)
        return nullptr;
    auto view = MM<GLViewLinux>::New();
    view->setWin(win);
    view->setSize(w, h);
    ::glfwSetWindowUserPointer(win, view);
    ::glfwMakeContextCurrent(win);
// callbacks
    ::glfwSetWindowSizeCallback(win, winsize_callback);
    ::glfwSetKeyCallback(win, key_callback);
    ::glfwSetMouseButtonCallback(win, mouse_button_callback);
    ::glfwSetScrollCallback(win, scroll_callback);
    ::glfwSetCursorPosCallback(win, cursor_position_callback);
    Assert(glewInit()==GLEW_OK, "init glew failed");
    {
        double x, y;
        ::glfwGetCursorPos(win, &x, &y);
        view->cursor_=Vec2((float)x, view->height_-(float)y);
    }
    return view;
}
// FIXME: how to make the plain callback
void GLViewLinux::winsize_callback(GLFWwindow *win, int width, int height)
{
    auto d = Director::getInstance();
    auto view = static_cast<GLViewLinux*>(::glfwGetWindowUserPointer(win));
    view->onResize(width, height);
    d->onWinSizeChanged();
}
class EventKeyboard_linux : public EventKeyboard
{friend class GLViewLinux;};
void GLViewLinux::key_callback(GLFWwindow *, int key, int , int action, int mods)
{
    EventKeyboard_linux e;
    switch(action){
        case GLFW_PRESS:
        e.action_ = KeyAction::KEY_PRESS;
        break;
        case GLFW_RELEASE:
        e.action_ = KeyAction::KEY_RELEASE;
        break;
        case GLFW_REPEAT:
        default:
            return;
    }
    e.mods_ = toKeyMods(mods);
    e.code_ = (KeyCode)key;
    Director::getInstance()->handleEvent(&e);
}
class EventMouse_linux : public EventMouse{
public:
    inline EventMouse_linux(MouseCode code):EventMouse(code){}
    friend class GLViewLinux;
};
void GLViewLinux::mouse_button_callback(GLFWwindow *win, int button, int action, int mods)
{
    GLViewLinux *view = (GLViewLinux*)::glfwGetWindowUserPointer(win);
    MouseCode code;
    MouseButton bnt;

    switch(action){
    case GLFW_PRESS:
        code = MouseCode::PRESS;
        break;
    case GLFW_RELEASE:
        code = MouseCode::RELEASE;
        break;
    default:
        // unknown mosue code, maybe a bug in glfw
        return;
    }
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        bnt = MouseButton::LEFT;
        view->pressedLeft = code==MouseCode::PRESS;
        if(view->pressedLeft){
            view->updateStartCursor();
        }
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        bnt = MouseButton::RIGHT;
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        bnt = MouseButton::MIDDLE;
        break;
    default:
        // unknown mouse button
        return;
    }

    EventMouse_linux e(code);
    e.setMouseButton(bnt);
    e.setKeyMods((KeyMods)toKeyMods(mods));

    auto const &cc = view->cursor_;
    e.setCursorPos(cc.x, cc.y);
    e.setStartPos(view->cursorStart_);
    if(action==GLFW_PRESS){
        auto const &p = view->updateStartCursor();
        e.setCursorPos(p.x, p.y);
    }
    Director::getInstance()->handleEvent(&e);
}

void GLViewLinux::scroll_callback(GLFWwindow *, double xoffset, double yoffset)
{
    EventMouse_linux e(MouseCode::SCROLL);
    e.setMouseButton(MouseButton::MIDDLE);
    e.setScroll((float)xoffset, (float)yoffset);
    Director::getInstance()->handleEvent(&e);
}

GLView * GLView::create(int w, int h)
{
    return GLViewLinux::createGLViewLinux(w, h, "");
}
// full screen
GLView * GLView::create()
{
    return GLViewLinux::createGLViewLinux(800, 480, "@fphoenix");
}
void GLViewLinux::cursor_position_callback(GLFWwindow* win, double x, double y)
{
    auto view = static_cast<GLViewLinux*>(::glfwGetWindowUserPointer(win));
    if(!view->pressedLeft)
        return ;
    view->cursor_.x = x;
    view->cursor_.y = view->height_-y;
    EventMouse_linux e(MouseCode::MOVE);
    e.setMouseButton(MouseButton::LEFT);
    e.setStartPos(view->cursorStart_);
    e.setCursorPos(view->cursor_);
    Director::getInstance()->handleEvent(&e);
}
