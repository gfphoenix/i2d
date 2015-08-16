/*
 * Node.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: wuhao
 */

#include <Director.hpp>
#include <IdleContainer.hpp>
#include <Node.hpp>
#include <Scene.hpp>
#include <math.h>
#include <config>
#include <algorithm>


Node::Node()
: color_(1,1,1,1)
, pos_(0.0f, 0.0f)
, size_(0.0f, 0.0f)
, anchor_(.5f, .5f)
, scaleX_(1)
, scaleY_(1)
, rotation_(0)
, zIndex_(0)
, am_(Director::getInstance()->getIdleContainer().getIdleActionManager())
, sched_(Director::getInstance()->getIdleContainer().getIdleScheduler())
, parent_(nullptr)
, scene_(nullptr)
, shader_(nullptr)
, running_(false)
, visible_(true)
, dirtyChildrenOrder_(false)
, dirtyTransform_(true)
{
    Debug("Node::Node\n");    
}

Node::~Node()
{
    Debug("Node::~Node\n");
}
void Node::beforeEnter()
{
    scene_ = parent_->scene_;
}

void Node::afterEnter()
{
    auto am = scene_->getActionManager();
    if(am_!=am){
        am_->moveNode(am, this);
        am_ = am;
    }
    auto sch = scene_->getScheduler();
    if(sch != sched_){
        // move scheduled task
        sched_->moveNode(sch, this);
        sched_ = sch;
    }
}
void Node::beforeExit()
{

}
void Node::afterExit()
{

}

void Node::onEnter(){
    Assert(!running_, "Node is running when onEnter");
    running_ = true;
    beforeEnter();
	for(const auto& child : children_){
		child->onEnter();
	}
   afterEnter();
//    Debug("running onEnter()\n");
}
void Node::onExit(){
    Assert(running_, "Node is not running when onExit");
//    Debug("running onExit()\n");
    beforeExit();
	for(const auto& child : children_){
		child->onExit();
	}
	running_ = false;
    afterExit();
}
void Node::toRemove()
{
    am_->removeActionsForNode(this);
    sched_->unscheduleNodeAll(this);
}

void Node::onRemove__()
{
}
void Node::updateWorldTransform__(const Mat3 &parentTransform)
{
    worldTransform_ = parentTransform * nodeToParentTransform_;
    onChangedTransformation();
}
// Mat3
void Node::updateNodeToParentTransform__()
{
	Mat3 &id = nodeToParentTransform_;
    double Cx=1, Sx=0, Cy=1, Sy=0;

    if (skew_.x != 0){
        Cx = cos(M_PI * skew_.x/180);
        Sx = sin(M_PI * skew_.x/180);
	}
    if(skew_.y != 0){
        Cy = cos(M_PI * skew_.y/180);
        Sy = sin(M_PI * skew_.y/180);
    }
	double ax = size_.x * anchor_.x;
	double ay = size_.y * anchor_.y;
    double ta = Cy * scaleX_;
    double tb = Sy * scaleX_;
    double tc =-Sx * scaleY_;
    double td = Cx * scaleY_;

	// 0,1,  3,4,  6,7
	id[0][0] = ta;
	id[0][1] = tb;
	id[1][0] = tc;
	id[1][1] = td;
	id[2][0] = ta*(-ax) + tc*(-ay) + pos_.x;
	id[2][1] = tb*(-ax) + td*(-ay) + pos_.y;
    dirtyTransform_ = false;
}
void Node::updateNodeToParentTransform0__()
{
    Mat3 &id = nodeToParentTransform_;
    double C=1, S=0;

    if (rotation_ != 0){
        C = cos(M_PI * rotation_/180);
        S = sin(M_PI * rotation_/180);
    }
    double ax = size_.x * anchor_.x;
    double ay = size_.y * anchor_.y;
    double ta = C * scaleX_;
    double tb = S * scaleX_;
    double tc =-S * scaleY_;
    double td = C * scaleY_;

    // 0,1,  3,4,  6,7
    id[0][0] = ta;
    id[0][1] = tb;
    id[1][0] = tc;
    id[1][1] = td;
    id[2][0] = ta*(-ax) + tc*(-ay) + pos_.x;
    id[2][1] = tb*(-ax) + td*(-ay) + pos_.y;
    dirtyTransform_ = false;
}
void Node::sortChildrenOrder__()
{
    std::stable_sort(children_.begin(), children_.end(),
            [](const Ref_ptr<Node> &l, const Ref_ptr<Node> &r)->bool{
                return l->zIndex_ < r->zIndex_;
            });
    dirtyChildrenOrder_ = false;
}
// tree node manipulation
// Add child
void Node::addChild(Node *child, int zIndex)
{
    Assert(child, "Node::addChild #! Child must not be nil");
    child->zIndex_ = zIndex;
    addChild(child);
}
void Node::addChild(Node *child)
{
    Assert(child, "Node::addChild #! Child must not be nil");
    Assert(child->parent_ == nullptr, "Node::addChild #! Parent must be nil");
    children_.push_back(Ref_ptr<Node>(child));
    child->parent_ = this;
    dirtyChildrenOrder_ = true;
    if(running_)
        child->onEnter();
}
//  Remove child
void Node::removeAll()
{
    int n=children_.size();
    while(n-->0){
        auto child = children_.back();
        if(child->running_)
            child->onExit();
        child->parent_ = nullptr;
        child->scene_ = nullptr;
        children_.pop_back();
    }
    dirtyChildrenOrder_ = false;
}
void Node::removeChild(Node *child)
{
    Assert(child != nullptr, "Node::removeChild #! child must not be nil");
    auto x = std::find(children_.cbegin(), children_.cend(), child);
    if(x==children_.cend())
        return;
    if(child->running_)
        child->onExit();
    toRemove();
    child->parent_ = nullptr;
    child->scene_ = nullptr;
    children_.erase(x);
    dirtyChildrenOrder_ = true;
}
void Node::remove()
{
    auto parent = this->parent_;
    if(parent != nullptr){
        parent->removeChild(this);
    }
}
// render && visit
void Node::DrawSelf(Renderer *)
{
    //Debug("Draw - %s\n", name_.c_str());
//    UNUSED(_);
}
void Node::visit(Renderer *scene, const Mat3 &parentTransform, bool parentTransformUpdated)
{
    if(!visible_)
        return ;
    bool dirty = dirtyTransform_ || parentTransformUpdated;
    if(dirtyTransform_)
        updateNodeToParentTransform__();
    if(dirty)
        updateWorldTransform__(parentTransform);

    if(children_.empty()){
        this->DrawSelf(scene);
        return;
    }

    auto child = children_.cbegin();
    for(;child!=children_.cend();child++){
        if((*child)->getZindex() >= 0)
            break;
        (*child)->visit(scene, worldTransform_, dirty);
    }
    this->DrawSelf(scene);
    for(; child != children_.cend(); child++){
        (*child)->visit(scene, worldTransform_, dirty);
    }
}
void Node::runAction(const Ref_ptr<Action> &action)
{
    Assert(action != nullptr, "action is nullptr");
    am_->addAction(this, action.get());
}
void Node::stopAction(Action *a)
{
    if(a == nullptr)
        return;
    Assert(a->getNode() != this, "Action's node is not the same");
    am_->removeAction(a);
}
void Node::pauseAction(Action *a)
{
    am_->pauseAction(a);
}
void Node::resumeAction(Action *a)
{
    am_->resumeAction(a);
}

void Node::clearActions()
{
    Assert(scene_ != nullptr, "Scene is nullptr");
    am_->removeActionsForNode(this);
}

