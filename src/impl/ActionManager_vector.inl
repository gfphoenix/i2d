#include <ActionManager.hpp>
#include <Node.hpp>
#include <algorithm>
#include <vector>

//namespace {
struct ActionNode 
{
    Ref_ptr<Action> action_;
    bool markRemove_;
    bool paused_;
    inline Node *getNode()const{ return action_->getNode();}
};
class ActionManager_vector : public ActionManager
{
    public:
        virtual void addAction(Node *associatedNode, Action *a, bool pause)override;
        virtual void removeAction(Action *a)override;
        virtual void removeActionsForNode(Node *node)override;
        virtual void pauseAction(Action *a)override;
        virtual void pauseActionsForNode(Node *node)override;
        virtual void resumeAction(Action *a)override;
        virtual void resumeActionsForNode(Node *node)override;
        virtual void moveNode(ActionManager *am, Node *node)override;
        virtual void update(float delta)override;
        std::vector<ActionNode> actions_;
};
typedef ActionManager_vector ActionManager_impl;
void ActionManager_vector::update(float delta)
{
    //for(auto it=actions_.begin(), end=actions_.end(); it!=end;){
    for(auto it=actions_.begin(); it!=actions_.end();){
        auto a = it->action_.get();
        Assert(a != nullptr, "Action is nullptr");
	if(it->markRemove_ || (!it->paused_ && !a->getNode()->isActionPaused() && run__(a, delta))){
            it =actions_.erase(it);
        }else{
            it++;
        }
    }
}
void ActionManager_vector::addAction(Node *associatedNode, Action *a, bool pause)
{
    Assert(a!=nullptr, "Action is not nullptr when added");
    a->setNode(associatedNode);
    actions_.push_back(ActionNode{Ref_ptr<Action>(a), false, pause});
}
void ActionManager_vector::removeAction(Action *a)
{
    Assert(a!=nullptr, "Action is nullptr when removed");
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
        if(it->action_ == a)
            it->markRemove_ = true;
    }
}
void ActionManager_vector::removeActionsForNode(Node *node)
{
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
	if(it->action_->getNode() == node)
            it->markRemove_ = true;
    }
}
void ActionManager_vector::pauseAction(Action *a)
{
    Assert(a != nullptr, "ActionManager_vector::pauseAction : action is nullptr");
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
        if(it->action_ == a)
            it->paused_ = true;
    }
}
void ActionManager_vector::pauseActionsForNode(Node *node)
{
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
	if(it->action_->getNode() == node)
            it->paused_ = true;
    }
}
void ActionManager_vector::resumeAction(Action *a)
{
    Assert(a != nullptr, "ActionManager_vector::pauseAction : action is nullptr");
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
        if(it->action_ == a)
            it->paused_ = false;
    }
}
void ActionManager_vector::resumeActionsForNode(Node *node)
{
    for(auto it=actions_.begin(),end=actions_.end(); it!=end; it++){
	if(it->action_->getNode() == node)
            it->paused_ = false;
    }
}
void ActionManager_vector::moveNode(ActionManager *am, Node *node)
{
    Assert(am!=nullptr && node!=nullptr, "action-manager or node must not be nil");
    printf("action size = %u\n", (unsigned)actions_.size());
    for(auto it=actions_.begin(); it!=actions_.end();){
	if(it->markRemove_){
	    it = actions_.erase(it);
	    continue;
	}
        auto a = it->action_.get();
	if(a->getNode()==node){
            am->addAction(node, a, it->paused_);
	    it = actions_.erase(it);
	    continue;
        }
	it++;
    }
}

//}
