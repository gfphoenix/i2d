#ifndef ACTION_HPP
#define ACTION_HPP
#include "Ref.hpp"
#include <vector>
#include <algorithm>
#include <stdio.h>
class Node;
class Action : public Ref
{
    public:
	virtual Action * clone()const=0;
	virtual Action * reverse()const=0;
	inline Node *getNode()const{return node_;}
	virtual void setNode(Node *node){node_=node;}
protected:
	virtual void update()=0;
	virtual void step(float dt)=0;
	//virtual void stop()=0;
	virtual void reset()=0;
	virtual bool isDone()const=0;
	Node *node_; // can be nullptr, which means action like a global function to run

	Action():node_(nullptr){}
	virtual ~Action(){}
	friend class ActionManager;
};

// just a conceptional action
class FiniteTimeAction : public Action
{
    public:
    virtual FiniteTimeAction * clone()const override=0;
    virtual FiniteTimeAction * reverse()const override=0;
    virtual void update()override=0;
    virtual void reset()override=0;
    virtual void step(float)override{}
    //virtual void stop()=0;
    virtual bool isDone()const override=0;
    virtual ~FiniteTimeAction(){}
};
class ActionInterval : public FiniteTimeAction
{
    public:
	inline float getDuration()const{return duration_;}
	float getPercent()const{return std::min(elapsed_/duration_, 1.0f);}
//protected:
	virtual void update(float percent)=0; // percent maybe greater than 1 or less than 0
	virtual void update()override{update(getPercent());}
	virtual void step(float dt)override;
	virtual bool isDone()const override{return elapsed_>=duration_;}
	virtual void reset()override{elapsed_=-1.f;}
	inline void init(float duration){
	    Assert(duration>0, "ActionInterval duration<0");
	    duration_=duration;
	    elapsed_=-1.f;
	}

	inline bool isFirstFrame()const{return elapsed_<0.f;}

protected:
	virtual ActionInterval * clone()const override=0;
	virtual ActionInterval * reverse()const override
	{
	    Assert(false, "no support");
	    return nullptr;
	}
	virtual ~ActionInterval(){}
	float elapsed_;
private:
	float duration_;
};
class ActionInstant : public FiniteTimeAction
{
    protected:
	virtual bool isDone()const override{return true;}

	virtual void reset()override{}
	virtual ~ActionInstant(){}
};
class ActionSet : public FiniteTimeAction
{
protected:
    virtual ~ActionSet(){}
    virtual void setNode(Node *node)override;
    virtual void reset()override;
    inline void init(const std::initializer_list<Ref_ptr<FiniteTimeAction>> &init_list)
    {
	actions_ = init_list;
    }
    inline void init(std::initializer_list<Ref_ptr<FiniteTimeAction>> &&init_list)
    {
	actions_ = std::move(init_list);
    }
public:
    std::vector<Ref_ptr<FiniteTimeAction>> actions_;
};

class Sequence : public ActionSet
{
public:
    static Sequence * create(const std::initializer_list<Ref_ptr<FiniteTimeAction>>  &list);
    static Sequence * create(std::initializer_list<Ref_ptr<FiniteTimeAction>> &&list);
    virtual Sequence * clone()const override;
    virtual Sequence * reverse()const override;
    protected:
    virtual ~Sequence(){}
    virtual void setNode(Node *node)override;
virtual void step(float dt)override{actions_[idx_]->step(dt);}
    virtual bool isDone()const override{return idx_==actions_.size();}
	virtual void update()override;
    virtual void reset()override;
    int idx_;
};
class Parallel : public ActionSet
{
public:
    static Parallel * create(const std::initializer_list<Ref_ptr<FiniteTimeAction>>  &list);
    static Parallel * create(std::initializer_list<Ref_ptr<FiniteTimeAction>> &&list);

    virtual Parallel * clone()const override;
    virtual Parallel * reverse()const override;
    protected:
    virtual void step(float dt)override;
    virtual void update()override;
    virtual void reset()override;
    virtual bool isDone()const override;
    inline void initData(size_t n)
    {

    }

    char *data; // 0~size()-1 : -1 => no exe; 0 => running or done. [size()] left running actions
};
class TimeScale : public FiniteTimeAction
{
public:
    static TimeScale * create(FiniteTimeAction * inner, float scale);
    protected:
	virtual void step(float dt)override{inner_->step(dt * scale_);}
	virtual void reset()override{inner_->reset();}
	virtual bool isDone()const override{return inner_->isDone();}
	virtual void setNode(Node *node)override{Action::setNode(node);inner_->setNode(node);}
	virtual TimeScale * clone()const override;
	virtual TimeScale * reverse()const override;
	virtual void update()override{inner_->update();}

    Ref_ptr<FiniteTimeAction> inner_;
    float scale_;
};
class Repeat : public FiniteTimeAction
{
public:
    static Repeat * create(FiniteTimeAction *inner, unsigned times);
    Repeat * clone() const override;
    Repeat * reverse() const override;
protected:
    void init(unsigned times){counter_=times;cc_=0;}
    virtual bool isDone()const override{return cc_==counter_;}
    virtual void setNode(Node *node)override{Action::setNode(node);inner_->setNode(node);}
    virtual void update()override;
    virtual void step(float dt)override{inner_->step(dt);}
    virtual void reset()override{cc_=0;inner_->reset();}
protected:
    unsigned cc_;
    unsigned counter_;
    Ref_ptr<FiniteTimeAction> inner_;

};

#endif // ACTION_HPP
