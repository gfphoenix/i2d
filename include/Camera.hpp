#ifndef _CAMERA_HPP
#define _CAMERA_HPP
#include "types.hpp"
#include "Ref.hpp"
#include <glm/gtc/matrix_transform.hpp>


class Camera : public Ref
{

    public:
    inline const Mat4 &getPV()const{return PV_;}
    inline Mat4 &getPV(){return PV_;}

    // affect camera position in world space
    inline void setEye(const Vec2 &eye)
    {
        this->eye_ = eye;
        update();
    }
    inline void setProj(const Mat4 &proj)
    {
        this->proj_ = proj;
        update();
    }

    // just affect projection matrix in eye space
    inline void Orth2d(float left, float right, float bottom, float top)
    {
        auto m = glm::ortho(left, right, bottom, top);
        setProj(m);
    }

protected:
    inline void update()
    {
        PV_ = proj_ * glm::lookAt(
                    Vec3(eye_, 1),
                    Vec3(eye_, -1),
                    Vec3(0, 1, 0)
                    );
    }
Mat4 PV_;
Mat4 proj_;
Vec2 eye_;
};

class OrthCamera : public Camera
{
    float left_;
    float right_;
    float bottom_;
    float top_;
    //float zoomFactor_;//
    public:
    OrthCamera(int width, int height)
        : left_(0)
          , right_(width)
          , bottom_(0)
          , top_(height)
    {
        PV_ = glm::ortho(left_, right_, bottom_, top_);
    }
    OrthCamera(){}
    void init(int width, int height)
    {
        left_ = bottom_ = 0;
        right_ = width;
        top_ = height;
        PV_ = glm::ortho(left_, right_, bottom_, top_);
    }
    //inline void setZoomFactor(float factor){zoomFactor_=factor;}
    //inline float getZoomFactor()const{return zoomFactor_;}
    inline void setZoomFactor(float factor){PV_[3][3] = 1.0/factor;}
    inline float getZoomFactor()const{return 1.0/PV_[3][3];}
    void move(float dx, float dy){
        left_   += dx;
        right_  += dx;
        bottom_ += dy;
        top_    += dy;
        update();
    }
    protected:
    inline void update(){
        float zoom = PV_[3][3];
        PV_ = glm::ortho(left_, right_, bottom_, top_);
        PV_[3][3] = zoom;
    }
};


#endif /* _CAMERA_HPP */
