#ifndef _CAMERA_HPP
#define _CAMERA_HPP
#include "types.hpp"
#include <glm/matrix.hpp>
//#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
// : public Ref
{

    public:
        inline Camera():zoom_(1){}
        inline const Mat4 &getPV()const{return PV_;}
        inline const Mat4 &getPV_1()const{return PV_1;}
        inline const Vec2 &getEye()const{return eye_;}
        inline Mat4 getViewMatrix()const
        {
            auto view = glm::lookAt(
                    Vec3(eye_, 0),
                    Vec3(eye_, -1),
                    Vec3(0, 1, 0)
                    );
            if(zoom_!=1.f){
                view[0][0] *= zoom_;
                view[0][1] *= zoom_;
                view[1][0] *= zoom_;
                view[1][1] *= zoom_;
                view[2][0] *= zoom_;
                view[2][1] *= zoom_;
                view[3][0] *= zoom_;
                view[3][1] *= zoom_;
            }
            return view;
        }
        inline float getZoomFactor()const{return zoom_;}
        // affect camera position in world space
        inline void setEye__(float x, float y){setEye__(Vec2(x,y));}
        inline void setEye__(const Vec2 &eye){this->eye_ = eye;}
        inline void setEye(float x, float y){setEye(Vec2(x,y));}
        inline void setEye(const Vec2 &eye)
        {
            setEye__(eye);
            update();
        }
        inline void setZoomFactor(float zoom)
        {
            if(zoom>0 && zoom!=zoom_){
                zoom_ = zoom;
                update();
            }
        }

        inline void setProj__(const Mat4 &proj){this->proj_ = proj;}
        inline void setProj(const Mat4 &proj)
        {
            setProj__(proj);
            update();
        }

        // just affect projection matrix in eye space
        inline void ortho2d__(float left, float right, float bottom, float top)
        {
            auto m = glm::ortho(left, right, bottom, top);
            setProj__(m);
        }
        inline void ortho2d(float left, float right, float bottom, float top)
        {
            ortho2d__(left, right, bottom, top);
            update();
        }

        inline void update()
        {
            PV_ = proj_ * getViewMatrix();
            PV_1 = glm::inverse(PV_);
        }
    protected:
        Mat4 PV_;
        Mat4 PV_1; // inverse matrix of pv
        Mat4 proj_;
        Vec2 eye_;
        float zoom_;
};

#endif /* _CAMERA_HPP */
