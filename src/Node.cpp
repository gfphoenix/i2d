/*
 * Node.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: wuhao
 */

#include <Director.hpp>
#include <Node.hpp>
#include <StageLayer.hpp>
#include <Scene.hpp>
#include <math.h>
#include <algorithm>


Node::Node()
: color_(1,1,1,1)
, pos_(0.0f, 0.0f)
, size_(0.0f, 0.0f)
, anchor_(.5f, .5f)
, scale_(1.f, 1.f)
, rotation_(0)
, zIndex_(0)
, parent_(nullptr)
, stage_(nullptr)
, shader_(nullptr)
, running_(false)
, visible_(true)
, dirtyChildrenOrder_(false)
, dirty_localTransform_(true)
{
    auto d = Director::getInstance();
    am_ = d->getIdleActionManager();
    sched_=d->getIdleScheduler();
}

Node::~Node()
{
}
Scene *Node::getScene()
{
    return stage_==nullptr ? nullptr : stage_->getScene();
}

void Node::onPause(){}
void Node::onResume(){}
//set stage or scene from parent, etc.
//if a node has no parent, it should override this method
void Node::beforeEnter()
{
    stage_ = parent_->stage_;
}
void Node::afterEnter()
{
    am_ = getScene()->getActionManager();
    sched_ = getScene()->getScheduler();
    if(hasListeners())
        getStageLayer()->setNeedOrderListeners();
}
void Node::beforeExit()
{
    if(hasListeners() && getStageLayer())
        getStageLayer()->setNeedOrderListeners();
}
void Node::afterExit()
{
    stage_=nullptr;
}

void Node::onEnter(){
    Assert(!running_, "Node is running when onEnter");
    running_ = true;
    beforeEnter();
    for(const auto& child : children_){
        child->onEnter();
    }
    afterEnter();
}
void Node::onExit(){
    Assert(running_, "Node is not running when onExit");
    beforeExit();
    for(const auto& child : children_){
        child->onExit();
    }
    afterExit();
    running_ = false;
}
void Node::toRemove()
{
    am_->removeActionsForNode(this);
    sched_->unscheduleAll(this);
}

void Node::onRemove__()
{
}
void Node::updateWorldTransform__(const Mat3 &parentTransform)
{
    worldTransform_ = parentTransform * nodeToParentTransform_;
    dirty_worldTransform_ = false;
    dirty_worldTransform_1_ = true;
    onChangedTransformation();
}
// Mat3
//void Node::updateNodeToParentTransform0__()
//{
//    Mat3 &id = nodeToParentTransform_;
//    double Cx=1, Sx=0, Cy=1, Sy=0;
//
//    if (skew_.x != 0){
//        Cx = cos(M_PI * skew_.x/180);
//        Sx = sin(M_PI * skew_.x/180);
//    }
//    if(skew_.y != 0){
//        Cy = cos(M_PI * skew_.y/180);
//        Sy = sin(M_PI * skew_.y/180);
//    }
//    double ax = size_.x * anchor_.x;
//    double ay = size_.y * anchor_.y;
//    double ta = Cy * scaleX_;
//    double tb = Sy * scaleX_;
//    double tc =-Sx * scaleY_;
//    double td = Cx * scaleY_;
//
//    // 0,1,  3,4,  6,7
//    id[0][0] = ta;
//    id[0][1] = tb;
//    id[1][0] = tc;
//    id[1][1] = td;
//    id[2][0] = ta*(-ax) + tc*(-ay) + pos_.x;
//    id[2][1] = tb*(-ax) + td*(-ay) + pos_.y;
//    dirty_localTransform_ = false;
//}
void Node::updateNodeToParentTransform_1__()
{
    if(dirty_localTransform_1_){
        nodeToParentTransform_1_ = glm::inverse(nodeToParentTransform_);
        dirty_localTransform_1_ = false;
    }
}
void Node::updateNodeToParentTransform__()
{
    Mat3 &M = nodeToParentTransform_;
    double C=1, S=0;

    if (rotation_ != 0){
        C = cos(M_PI * rotation_/180);
        S = sin(M_PI * rotation_/180);
    }
    auto const &size = getSize();
    auto const &anchor = getAnchor();
    double ax = size.x * anchor.x;
    double ay = size.y * anchor.y;
    double ta = C * scale_.x;
    double tb = S * scale_.x;
    double tc =-S * scale_.y;
    double td = C * scale_.y;

    // 0,1,  3,4,  6,7
    M[0][0] = ta;
    M[0][1] = tb;
    M[1][0] = tc;
    M[1][1] = td;
    M[2][0] = ta*(-ax) + tc*(-ay) + pos_.x;
    M[2][1] = tb*(-ax) + td*(-ay) + pos_.y;
    dirty_localTransform_ = false;
    // reverse matrix should update, world bit will be set follow
    dirty_localTransform_1_ = true;
    dirty_worldTransform_ = true;
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
    int n=(int)children_.size();
    while(n-->0){
        auto child = children_.back();
        if(child->running_)
            child->onExit();
        child->parent_ = nullptr;
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
}
void Node::visit(Renderer *renderer, const Mat3 &parentTransform, bool parentTransformUpdated)
{
    if(!visible_)
        return ;
    if(dirtyChildrenOrder_)
        sortChildrenOrder__();
    bool dirty = dirty_localTransform_ || parentTransformUpdated;
    if(dirty_localTransform_)
        updateNodeToParentTransform__();
    if(dirty)
        updateWorldTransform__(parentTransform);

    if(children_.empty()){
        this->DrawSelf(renderer);
        return;
    }

    auto child = children_.cbegin();
    for(;child!=children_.cend();child++){
        if((*child)->getZindex() >= 0)
            break;
        (*child)->visit(renderer, worldTransform_, dirty);
    }
    this->DrawSelf(renderer);
    for(; child != children_.cend(); child++){
        (*child)->visit(renderer, worldTransform_, dirty);
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
    Assert(a->getNode() == this, "Action's node is not the same");
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
    am_->removeActionsForNode(this);
}
void Node::addEventListener(EventListener *l)
{
    auto ok = listeners_.addListener(l);
    Assert(ok, "Eventlistener register failed");
    l->setRegistered();
    l->setNode(this);
    auto stage = getStageLayer();
    if(stage){
        stage->setNeedOrderListeners();
    }
}
void Node::removeEventListener(EventListener *l)
{
    auto ok = listeners_.removeListener(l);
    //false means failed, not owned by this node
    if(!ok)
        return;
    auto stage = getStageLayer();
    if(stage){
        stage->setNeedOrderListeners();
    }
}
void Node::clearEventListeners()
{
    listeners_.clear();
}
bool Node::updateWorldTransformRec__()
{
    bool dirty;
    if(parent_==nullptr){
        dirty = dirty_localTransform_;
        if(dirty_localTransform_)
            updateNodeToParentTransform__();
        if(dirty_worldTransform_){
            worldTransform_ = nodeToParentTransform_;
            dirty_worldTransform_ = false;
        }
        return dirty;
    }
    auto tmp = parent_->updateWorldTransformRec__();
    dirty = tmp || dirty_localTransform_;
    if(dirty_localTransform_)
        updateNodeToParentTransform__();
    dirty_worldTransform_ = dirty_worldTransform_ || tmp;
    if(dirty_worldTransform_)
        updateWorldTransform__(parent_->worldTransform_);
    return dirty;
}

Vec2 Node::toWorld(const Vec2 &local)
{
    updateWorldTransformRec__();
    auto v = worldTransform_ * Vec3(local, 1);
    return Vec2(v);
}
Vec2 Node::toLocal(const Vec2 &world)
{
    updateWorldTransformRec__();
    if(dirty_worldTransform_1_){
        worldTransform_1_ = glm::inverse(worldTransform_);
        dirty_worldTransform_1_ = false;
    }
    auto v = worldTransform_1_ * Vec3(world, 1);
    return Vec2(v);
}
bool Node::hit(const Vec2 &local)
{
    auto const &size = getSize();
    return local.x>=0 && local.x<=size.x && local.y>=0 && local.y<=size.y;
}
