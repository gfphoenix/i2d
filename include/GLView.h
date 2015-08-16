#ifndef GLVIEW_H_
#define GLVIEW_H_
#include "types.hpp"
#include "Ref.hpp"

class GLView : public Ref
{
    public:
        virtual const std::string getInfo()const{return "GLView";}
        static GLView *create();
        virtual ~GLView(){}
        virtual int getWinWidth()const=0;
        virtual int getWinHeight()const=0;
        inline iVec2 getWinSize()const{return iVec2(getWinWidth(),getWinHeight());}
        virtual bool shouldClose()const=0;
        virtual void swapBuffer()=0;
        virtual void pollEvents()=0;

        const iVec2 &getViewOrigin()const{return view_.oxy;}
        const iVec2 &getViewSize()const{return view_.size;}
        void setDesignResolution(int width, int height, ResolutionPolicy policy=ResolutionPolicy::FULL)
        {
            int W = getWinWidth();
            int H = getWinHeight();
            Assert(W>0 && H>0 && width>0 && height>0, "width & height must be greater than 0");
            designResolution_.x = width;
            designResolution_.y = height;
            policy_ = policy;
            updateViewport();
        }
        void updateViewport()
        {
            int W = getWinWidth();
            int H = getWinHeight();
            auto width = designResolution_.x;
            auto height = designResolution_.y;
            float f1 = (float)W/(float)H;
            float f2 = (float)width/(float)height;
            switch(policy_){
                case ResolutionPolicy::FULL:
                    view_.setOrigin(0, 0);
                    view_.setSize(W, H);
                    break;
                case ResolutionPolicy::KEEP:
                    if(f1 > f2){//屏幕太宽，左右两边黑边
                        double t = H * width;
                        t /= (double)height;
                        double x = (W-t)/2;

                        view_.oxy[0] = (int)(x+.5);//round
                        view_.size[0] = (int)(t+.5); // round width
                        view_.oxy[1] = 0;
                        view_.size[1] = H;
                    }else{
                        double t = W * height;
                        t /= (double)width;
                        double y = (H-t)/2;

                        view_.oxy[1] = (int)(y+.5);
                        view_.size[1] = (int)(t+.5);
                        view_.oxy[0]=0;
                        view_.size[0] = W;
                    }
                    break;
                case ResolutionPolicy::NO_BOARDER:
                    if(f1 < f2){//屏幕太窄，上下两边部分被截断
                        double t = H * width;
                        t /= (double)height;
                        double x = (W-t)/2;

                        view_.oxy[0] = (int)(x+.5);//round
                        view_.size[0] = (int)(t+.5); // round width
                        view_.oxy[1] = 0;
                        view_.size[1] = H;
                    }else{
                        double t = W * height;
                        t /= (double)width;
                        double y = (H-t)/2;

                        view_.oxy[1] = (int)(y+.5);
                        view_.size[1] = (int)(t+.5);
                        view_.oxy[0]=0;
                        view_.size[0] = W;
                    }
                    break;
                default:
                    break;
            }
            Debug("view port (%d, %d) - %d x %d\n",
                    view_.oxy[0], view_.oxy[1], view_.size[0], view_.size[1]);
            ::glViewport(view_.oxy[0], view_.oxy[1], view_.size[0], view_.size[1]);
        }
        inline const iVec2 &getDesignResolution()const{return designResolution_;}
        virtual void onResize(int width, int height){}
    protected:
        iRect view_;
        iVec2 designResolution_;
        ResolutionPolicy policy_;
};

#endif /* GLVIEW_H_ */
