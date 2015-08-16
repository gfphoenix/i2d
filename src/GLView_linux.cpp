#include "GLView.h"
#include <GLView_linux.hpp>
//#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"
#include <mm.hpp>
#include <Director.hpp>

static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    switch(action){
        case GLFW_PRESS:
        case GLFW_RELEASE:
        case GLFW_REPEAT:
        default:
            break;
    }
}
static void mouse_button_callback(GLFWwindow *win, int button, int action, int mods)
{
}
static void scroll_callback(GLFWwindow *win, double xoffset, double yoffset)
{
}

namespace 
{
    static void resize(GLFWwindow *win, int width, int height);
    class GLViewLinux : public GLView
    {
        private:
            GLFWwindow *win_;
            int width_;
            int height_;

            virtual int getWinWidth()const override;
            virtual int getWinHeight()const override;
            virtual bool shouldClose()const override;
            virtual void swapBuffer()override;
            virtual void pollEvents()override;
//            virtual void setDesignResolution(int width, int height, ResolutionPolicy policy)override;

            GLViewLinux();
            friend class MM<GLViewLinux>;
            friend void resize(GLFWwindow *win, int width, int height);
        public:
            inline virtual void onResize(int w, int h)override{setSize(w,h);updateViewport();}
            inline void setSize(int w, int h){width_=w;height_=h;}
            inline void setWin(GLFWwindow *win){win_ = win;}
            virtual ~GLViewLinux();
    };
    GLViewLinux::GLViewLinux()
        : GLView()
    {}
    GLViewLinux::~GLViewLinux()
    {
        ::glfwTerminate();
    }
    bool GLViewLinux::shouldClose()const
    {
        return ::glfwWindowShouldClose(win_);
    }
    void GLViewLinux::swapBuffer()
    {
        return ::glfwSwapBuffers(win_);
    }
    void GLViewLinux::pollEvents()
    {
        ::glfwPollEvents();
    }
    int GLViewLinux::getWinWidth()const
    {
        return width_;
    }
    int GLViewLinux::getWinHeight()const
    {
        return height_;
    }
    // FIXME: how to make the plain callback 
    static void resize(GLFWwindow *win, int width, int height)
    {
        auto &view = Director::getInstance()->getGLView();
        if(view){
            view->onResize(width, height);
        }
    }
    static GLViewLinux *createGLViewLinux(int w, int h, const char *title)
    {
        ::glfwInit();
        ::glfwWindowHint(GLFW_SAMPLES, 4);

        //::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        auto win = ::glfwCreateWindow(w, h, title, nullptr, nullptr);
        if(win == nullptr)
            return nullptr;
        ::glfwMakeContextCurrent(win);
        ::glfwSetWindowSizeCallback(win, resize);
        ::glfwSetKeyCallback(win, key_callback);
        ::glfwSetMouseButtonCallback(win, mouse_button_callback);
        ::glfwSetScrollCallback(win, scroll_callback);
        Assert(glewInit()==GLEW_OK, "init glew failed");
        auto view = MM<GLViewLinux>::New();
        view->setWin(win);
        view->setSize(w, h);
        return view;
    }
} 
GLView * GLView::create()
{
    return createGLViewLinux(300, 400, "");
}
