#ifndef _ACTION_INTERVAL_HPP_
#define _ACTION_INTERVAL_HPP_
#include "ActionManager.hpp"
#include "Action.hpp"
#include "Node.hpp"
#include "Ref.hpp"

class Delay : public ActionInterval
{
    public:
        static Delay * create(float delay);
    protected:
        Delay * clone() const override{return create(getDuration());}
        Delay * reverse() const override{return create(getDuration());}
        virtual void update()override{}
        virtual void update(float)override{}
};

/*
class B1 : public ActionInterval
{
public:
    virtual ActionInterval * clone() const override;
    virtual ActionInterval * reverse() const override;

protected:
    virtual void reset()override=0;
    virtual void update(float percent)override=0;
    virtual void setNode(Node *node)override=0;
    inline void init(float duration, float by){
	ActionInterval::init(duration);
	by_=by;
    }
    float start_;
    float by_;
};
*/

class RotateBy : public ActionInterval
{
    public:
        static RotateBy * create(float duration, float degree);
        virtual RotateBy * clone()const override{return create(getDuration(), by_);}
        virtual RotateBy * reverse()const override{return create(getDuration(), -by_);}
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        inline void init(float duration, float by){
            ActionInterval::init(duration);
            by_=by;
        }
    float start_;
    float by_;
};
class MoveBy : public ActionInterval
{
    public:
        static MoveBy * create(float duration, float dx, float dy);
        static MoveBy * create(float duration, const Vec2 &dxy);
        virtual MoveBy * clone()const override{return create(getDuration(), dXY_);}
        virtual MoveBy * reverse()const override{return create(getDuration(), -dXY_);}
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        inline void init(float duration, const Vec2 &dxy){
            ActionInterval::init(duration);
            dXY_ = dxy;
        }
        Vec2 dXY_;
        Vec2 start_;
};
class ScaleBy : public ActionInterval
{
public:
    static ScaleBy * create(float duration, float dScaleX, float dScaleY);
    static ScaleBy * create(float duration, const Vec2 &dScale);
    virtual ScaleBy *clone() const override{return create(getDuration(), d2_);}
    virtual ScaleBy *reverse()const override{return create(getDuration(), -d2_);}
protected:
    virtual void reset()override;
    virtual void update(float percent)override;
    virtual void setNode(Node *node)override;
    inline void init(float duration, const Vec2 &dScale){
	ActionInterval::init(duration);
	d2_ = dScale;
    }
    Vec2 start_;
    Vec2 d2_;
};
class SizeBy : public ActionInterval
{
    public:
        static SizeBy * create(float duration, float dw, float dh);
        static SizeBy * create(float duration, const Vec2 &dSize);
        virtual SizeBy * clone()const override{return create(getDuration(), dSize_);}
        virtual SizeBy * reverse()const override{return create(getDuration(), -dSize_);}
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        void init(float duration, const Vec2 &dSize){
            ActionInterval::init(duration);
            dSize_ = dSize;
        }
        Vec2 dSize_;
        Vec2 oldSize_;
};
// RotateTo, MoveTo, SizeTo, ScaleTo, ... *To are not
// supported Action::reverse(). Doing this will result
// an Assert failure.
class RotateTo : public ActionInterval
{
    public:
        static RotateTo * create(float duration, float to);
        virtual RotateTo * clone()const override{return create(getDuration(), to_);}
        //virtual RotateTo * reverse()const override;
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        inline void init(float duration, float to){
            ActionInterval::init(duration);
            to_ = to;
        }
        float dr_; // the init delta degree
        float to_;
};
class MoveTo : public ActionInterval
{
    public:
        static MoveTo * create(float duration, float x, float y);
        static MoveTo * create(float duration, const Vec2 &to);
        virtual MoveTo * clone()const override{return create(getDuration(), to_);}
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        inline void init(float duration, const Vec2 &to){
            ActionInterval::init(duration);
            to_ = to;
        }
        Vec2 dXY_;
        Vec2 to_;
};
class ScaleTo : public ActionInterval
{
public:
    static ScaleTo * create(float duration, float sx, float sy);
    static ScaleTo * create(float duration, const Vec2 &to);
    virtual ScaleTo * clone()const override{return create(getDuration(), to_);}
protected:
    virtual void reset()override;
    virtual void update(float percent)override;
    virtual void setNode(Node *node)override;
    inline void init(float duration, const Vec2 &to){
	ActionInterval::init(duration);
	to_ = to;
    }
    Vec2 d2_;
    Vec2 to_;
};

class SizeTo : public ActionInterval
{
    public:
        static SizeTo * create(float duration, float w, float h);
        static SizeTo * create(float duration, const Vec2 &to);
        virtual SizeTo * clone()const override{return create(getDuration(), to_);}
    protected:
        virtual void reset()override;
        virtual void update(float percent)override;
        virtual void setNode(Node *node)override;
        inline void init(float duration, const Vec2 &to){
            ActionInterval::init(duration);
            to_ = to;
        }
        Vec2 dSize_;
        Vec2 to_;
};

#endif
