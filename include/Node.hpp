#ifndef NODE_HPP_
#define NODE_HPP_

#include "types.hpp"
#include "Ref.hpp"
#include <ActionManager.hpp>
#include <Scheduler.hpp>
#include <vector>
class Renderer;
class Scheduler;
class Shader;
class Scene;
class StageLayer;

// basic Scene Graph Tree Node for render
class Node : public Ref{
public:
	Node();
	virtual ~Node();
    virtual const std::string getInfo()const{return std::string("Node")+name_;}
    inline const std::string & getName()const{return name_;}
    inline void setName(const std::string &name){name_=name;}
    inline void setName(std::string &&name){name_=std::move(name);}
    //inline Color4 &getColor(){return color_;}
    inline const Color4 &getColor()const{return color_;}
    inline Color4 getColorCp()const{return color_;}
    inline void setColor(const Color4 &c4){color_=c4;onChangedColor();}
    inline void setColor(const Color3 &c3){setColor(Color4(c3,1));}
    inline void setColor(u8 r, u8 g, u8 b, u8 a){
        setColor(Color4((float)r/255.0,(float)g/255.0,(float)b/255.0,(float)a/255.0));
    }
    inline void setColor(u8 r, u8 g, u8 b){
        setColor(Color4((float)r/255.0,(float)g/255.0,(float)b/255.0,1));
    }

	inline float getX()const{return pos_.x;}
	inline float getY()const{return pos_.y;}
    inline const Vec2 &getPosition()const{return pos_;}
    inline float getWidth()const{return size_.x;}
    inline float getHeight()const{return size_.y;}
    inline const Vec2 &getSize()const{return size_;}
	inline float getAnchorX()const{return anchor_.x;}
	inline float getAnchorY()const{return anchor_.y;}
    inline const Vec2 &getAnchor()const{return anchor_;}
	inline float getScaleX()const{return scaleX_;}
	inline float getScaleY()const{return scaleY_;}
    inline Vec2 getScale()const{return Vec2(scaleX_,scaleY_);}
    inline float getSkewX()const{return skew_.x;}
    inline float getSkewY()const{return skew_.y;}
    inline Vec2 getSkew()const{return skew_;}
    inline float getRotation()const{return rotation_;}
	inline int   getZindex()const{return zIndex_;}

	inline void setX(float x){pos_.x=x;dirtyTransform_=true;}
	inline void setY(float y){pos_.y=y;dirtyTransform_=true;}
	inline void setPosition(float x, float y){pos_.x=x;pos_.y=y;dirtyTransform_=true;}
    inline void setPosition(const Vec2 &pos){pos_=pos;dirtyTransform_=true;}
    inline void setWidth(float w){size_.x=w;dirtyTransform_=true;}
    inline void setHeight(float h){size_.y=h;dirtyTransform_=true;}
    inline void setSize(float w, float h){size_.x=w;size_.y=h;dirtyTransform_=true;}
    inline void setSize(const Vec2 &size){size_=size;dirtyTransform_=true;}
	inline void setAnchorX(float anchorX){anchor_.x=anchorX;dirtyTransform_=true;}
	inline void setAnchorY(float anchorY){anchor_.y=anchorY;dirtyTransform_=true;}
	inline void setAnchor(float ax, float ay){anchor_.x=ax;anchor_.y=ay;dirtyTransform_=true;}
    inline void setAnchor(const Vec2 &anchor){anchor_=anchor;dirtyTransform_=true;}
	inline void setScaleX(float scaleX){scaleX_=scaleX;dirtyTransform_=true;}
	inline void setScaleY(float scaleY){scaleY_=scaleY;dirtyTransform_=true;}
    inline void setScale(float scale){scaleX_=scaleY_=scale;dirtyTransform_=true;}
	inline void setScale(float sx, float sy){scaleX_=sx;scaleY_=sy;dirtyTransform_=true;}
    inline void setScale(const Vec2 &v){scaleX_=v[0];scaleY_=v[1];dirtyTransform_=true;}
    inline void setSkewX(float skewX){skew_.x=skewX;dirtyTransform_=true;}
    inline void setSkewY(float skewY){skew_.y=skewY;dirtyTransform_=true;}
    inline void setSkew(const Vec2 &skew){skew_=skew;dirtyTransform_=true;}
    inline void setRotation(float rotation){rotation_=rotation;dirtyTransform_=true;}
	inline void setZindex(int zIndex){
	    zIndex_=zIndex;
	    if(parent_)
		parent_->dirtyChildrenOrder_=true;
	}

	inline bool isRunning()const{return running_;}
	inline bool isVisible()const{return visible_;}
	inline void setVisible(bool visible){visible_=visible;}
    inline bool isActionPaused()const{return pauseAction_;}
    inline void setActionPaused(bool pause){pauseAction_=pause;}
    inline bool isSchedulerPaused()const{return pauseScheduler_;}
    inline void setSchedulerPaused(bool pause){pauseScheduler_=pause;}

    //inline bool operator < (const Node &x)const{return zIndex_<x.getZindex();}

	// tree op
	typedef std::vector<Ref_ptr<Node>> Vector;
    void addChild(Node *child);
    void addChild(Node *child, int zIndex);
	void removeChild(Node *child);
	void remove(); // remove self from parent
	void removeAll(); // remove all children
	inline const Vector &getChildren()const{return children_;}
	inline int getChildrenSize()const{return children_.size();}
	inline Node* getParent()const{return parent_;}
    inline void setDelaySortChildren(bool delay){if(!delay&&dirtyChildrenOrder_)sortChildrenOrder__();}

	virtual void onEnter();
	virtual void onExit();
	virtual void beforeEnter();
	virtual void afterEnter();
	virtual void beforeExit();
	virtual void afterExit();
    virtual void toRemove();
    // Actions
    void runAction(const Ref_ptr<Action> &action);
    void stopAction(Action *);
    void pauseAction(Action *);
    void resumeAction(Action *);
    void clearActions();
    // scheduler
    inline void scheduleUpdate(){sched_->scheduleUpdate(this);}
    inline void unScheduleUpdate(){sched_->unscheduleUpdate(this);}
    inline void schedule(const Scheduler::bFunc &func, const std::string &key)
    {sched_->scheduleNode(this, key, func);}
    inline void schedule(const Scheduler::bFunc &func, const std::string &key, float delay, float interval, unsigned repeat)
    {sched_->scheduleNode(this, key, func, delay, interval, repeat);}
    void unSchedule(const std::string &key){sched_->unscheduleNodeOne(this, key);}
    virtual bool update(float dt){return true;}

    // Render related, need parent's color ?
    virtual void DrawSelf(Renderer *_);
    void visit(Renderer *scene, const Mat3 &parentTransform, bool parentTransformUpdated);
	inline Shader *getShader()const {return shader_;}
	inline void setShader(Shader *shader){shader_=shader;}
    // matrix
    virtual void onChangedTransformation(){}
    virtual void onChangedColor(){}
	inline const Mat3 &getWorldTransform()const{return worldTransform_;}
	inline const Mat3 &getNodeToParentTransform()const{return nodeToParentTransform_;}
//protected:
    virtual void onRemove__();
	void updateNodeToParentTransform__();
void updateNodeToParentTransform0__();
    void updateWorldTransform__(const Mat3 &parentTransform);
    void sortChildrenOrder__(); // MUST BE stable sort
    
// members
    std::string name_;
    Color4 color_;
    Vec2 pos_;
    Vec2 size_;
    Vec2 anchor_;
	float scaleX_;
	float scaleY_;
	Vec2 skew_;
	float rotation_; // z-axis rotation in degree
	int zIndex_;

	//
	ActionManager *am_;
	Scheduler *sched_;
	// tree related, weak reference
	Node *parent_;
    StageLayer *stage;
	Vector children_;

	Shader *shader_;

    friend class Scene;


//	bool running_;
//	bool visible_;
//    bool pauseAction_;
//    bool pauseScheduler_;
//
//    bool dirtyChildrenOrder_;
//	bool dirtyTransform_; //*/ if true, need to recalculate matrix
    Mat3 worldTransform_;
	Mat3 nodeToParentTransform_; // node to parent transform matrix
    unsigned running_:1;
    unsigned visible_:1;
    unsigned pauseAction_:1;
    unsigned pauseScheduler_:1;
    unsigned dirtyChildrenOrder_:1;
    unsigned dirtyTransform_:1;
};

//} /* namespace fphoenix */

#endif /* NODE_HPP_ */
