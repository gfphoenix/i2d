#ifndef NODE_HPP_
#define NODE_HPP_
#include "config.hpp"
#include "types.hpp"
#include "Ref.hpp"
#include <ActionManager.hpp>
#include "ListenerVector.hpp"
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
    virtual const Color4 &getColor()const{return color_;}
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
    inline float getWidth()const{return size_.x;}
    inline float getHeight()const{return size_.y;}
	inline float getAnchorX()const{return anchor_.x;}
	inline float getAnchorY()const{return anchor_.y;}
    //inline float getSkewX()const{return skew_.x;}
    //inline float getSkewY()const{return skew_.y;}
	inline float getScaleX()const{return scale_.x;}
	inline float getScaleY()const{return scale_.y;}

    inline const Vec2 &getPosition()const{return pos_;}
    virtual const Vec2 &getSize()const{return size_;}
    virtual const Vec2 &getAnchor()const{return anchor_;}
    inline const Vec2 &getScale()const{return scale_;}
    //inline const Vec2 &getSkew()const{return skew_;}
    inline float getRotation()const{return rotation_;}
	inline int   getZindex()const{return zIndex_;}

	inline void setX(float x){pos_.x=x;dirty_localTransform_=true;}
	inline void setY(float y){pos_.y=y;dirty_localTransform_=true;}
	inline void setPosition(float x, float y){pos_.x=x;pos_.y=y;dirty_localTransform_=true;}
    inline void setPosition(const Vec2 &pos){pos_=pos;dirty_localTransform_=true;}
    inline void setWidth(float w){size_.x=w;dirty_localTransform_=true;}
    inline void setHeight(float h){size_.y=h;dirty_localTransform_=true;}
    inline void setSize(float w, float h){size_.x=w;size_.y=h;dirty_localTransform_=true;}
    inline void setSize(const Vec2 &size){size_=size;dirty_localTransform_=true;}
	inline void setAnchorX(float anchorX){anchor_.x=anchorX;dirty_localTransform_=true;}
	inline void setAnchorY(float anchorY){anchor_.y=anchorY;dirty_localTransform_=true;}
	inline void setAnchor(float ax, float ay){anchor_.x=ax;anchor_.y=ay;dirty_localTransform_=true;}
    inline void setAnchor(const Vec2 &anchor){anchor_=anchor;dirty_localTransform_=true;}
	inline void setScaleX(float scaleX){scale_.x=scaleX;dirty_localTransform_=true;}
	inline void setScaleY(float scaleY){scale_.y=scaleY;dirty_localTransform_=true;}
    inline void setScale(float scale){scale_.x=scale_.y=scale;dirty_localTransform_=true;}
	inline void setScale(float sx, float sy){scale_.x=sx;scale_.y=sy;dirty_localTransform_=true;}
    inline void setScale(const Vec2 &scale){scale_=scale;dirty_localTransform_=true;}
    //inline void setSkewX(float skewX){skew_.x=skewX;dirty_localTransform_=true;}
    //inline void setSkewY(float skewY){skew_.y=skewY;dirty_localTransform_=true;}
    //inline void setSkew(float skew){skew_.x=skew_.y=skew;dirty_localTransform_=true;}
	//inline void setSkew(float sx, float sy){skew_.x=sx;skew_.y=sy;dirty_localTransform_=true;}
    //inline void setSkew(const Vec2 &skew){skew_=skew;dirty_localTransform_=true;}
    inline void setRotation(float rotation){rotation_=rotation;dirty_localTransform_=true;}
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
    inline void sortChildrenOrder(){if(dirtyChildrenOrder_)sortChildrenOrder__();}
    inline void setDelaySortChildren(bool delay){if(!delay&&dirtyChildrenOrder_)sortChildrenOrder__();}

    virtual Scene *getScene();
    virtual StageLayer *getStageLayer(){return stage_;}


    virtual void onEnter();
    virtual void onExit();
    virtual void beforeEnter();
    virtual void afterEnter();
    virtual void beforeExit();
    virtual void afterExit();
    virtual void onPause();
    virtual void onResume();
    virtual void toRemove();
    // Actions
    void runAction(const Ref_ptr<Action> &action);
    void stopAction(Action *);
    void pauseAction(Action *);
    void resumeAction(Action *);
    void clearActions();
    // Listeners
    inline int getListenerNumber()const
    {return listeners_.v_?(int)listeners_.v_->size():0;}
    inline bool hasListeners()const{return getListenerNumber()>0;}
    void addEventListener(EventListener *l);
    void removeEventListener(EventListener *l);
    void clearEventListeners();
    virtual bool hit(const Vec2 &local);

    // scheduler
    //inline void scheduleUpdate(){sched_->scheduleUpdate(this);}
    //inline void unScheduleUpdate(){sched_->unscheduleUpdate(this);}
    inline void schedule(const Scheduler::bFunc &func, const std::string &key)
    {sched_->schedule(this, key, func);}
    inline void schedule(const Scheduler::bFunc &func, const std::string &key, float delay, float interval, unsigned repeat)
    {sched_->schedule(this, key, func, delay, interval, repeat);}
    void unSchedule(const std::string &key){sched_->unscheduleOne(this, key);}
    virtual bool update(float dt){UNUSED(dt);return true;}

    // Render related, need parent's color ?
    virtual void DrawSelf(Renderer *_);
    void visit(Renderer *renderer, const Mat3 &parentTransform, bool parentTransformUpdated);
    inline Shader *getShader()const {return shader_;}
    inline void setShader(Shader *shader){shader_=shader;}
    // matrix
    virtual void onChangedTransformation(){}
    virtual void onChangedColor(){}
    inline const Mat3 &getWorldTransform()
    {
        return worldTransform_;
    }
    inline const Mat3 &getWorldTransform_1()
    {
        return worldTransform_1_;
    }
    inline const Mat3 &getNodeToParentTransform()
    {
        if(dirty_localTransform_)
            updateNodeToParentTransform__();
        return nodeToParentTransform_;
    }
    inline const Mat3 &getNodeToParentTransform_1()
    {
        if(dirty_localTransform_)
            updateNodeToParentTransform__();
        if(dirty_localTransform_1_)
            updateNodeToParentTransform_1__();
        return nodeToParentTransform_1_;
    }
    Vec2 toWorld(const Vec2 &local);
    Vec2 toLocal(const Vec2 &world);
    inline Vec2 toWorldAR(const Vec2 &localAR){return toWorld(getAnchor()*getSize()+localAR);}
    inline Vec2 toLocalAR(const Vec2 &world){return toLocal(world)-(getAnchor()*getSize());}
    inline Vec2 toWorld(float localX, float localY){return toWorld(Vec2(localX,localY));}
    inline Vec2 toLocal(float worldX, float worldY){return toLocal(Vec2(worldX,worldY));}
protected:
    virtual void onRemove__();
    bool updateWorldTransformRec__();
    void updateNodeToParentTransform__();
    void updateNodeToParentTransform_1__();
    void updateWorldTransform__(const Mat3 &parentTransform);
    void sortChildrenOrder__(); // MUST BE stable sort
    inline const std::vector<Ref_ptr<EventListener>> &getEventListeners()
    {
        return listeners_.getEventListeners();
    }

    // members
    std::string name_;
    Color4 color_;
    Vec2 pos_;
    Vec2 size_;
    Vec2 anchor_;
    Vec2 scale_;
    //Vec2 skew_;
    float rotation_; // z-axis rotation in degree
    int zIndex_;

    //
    ActionManager *am_;
    ListenerVector listeners_;
    Scheduler *sched_;
    // tree related, weak reference
    Node *parent_;
    StageLayer *stage_;
    Vector children_;

    Shader *shader_;

    friend class Scene;
    friend class StageLayer;

    Mat3 worldTransform_;
    Mat3 nodeToParentTransform_; // node to parent transform matrix
    Mat3 worldTransform_1_;
    Mat3 nodeToParentTransform_1_;
    unsigned running_:1; // if add to a running tree
    unsigned paused_ :1; // 
    unsigned visible_:1;
    unsigned pauseAction_:1;
    unsigned pauseScheduler_:1;
    unsigned dirtyChildrenOrder_:1;
    unsigned dirty_localTransform_:1;
    unsigned dirty_localTransform_1_:1;
    unsigned dirty_worldTransform_:1;
    unsigned dirty_worldTransform_1_:1;

};

//} /* namespace fphoenix */

#endif /* NODE_HPP_ */
