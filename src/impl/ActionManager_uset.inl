#include <ActionManager.hpp>
#include <unordered_set>

namespace {
struct myhash {
    size_t operator()(const Ref_ptr<Action> &a)const{
        return std::hash<Node*>()(a->getNode());
    }
};
class ActionManager_uset : public ActionManager
{
    public:
	virtual void addAction(Node *associatedNode, Action *a)override;
        virtual void removeAction(Action *a)override;
        virtual void removeActionsForNode(Node *node)override;
        virtual void pauseAction(Action *a)override;
        virtual void pauseActionsForNode(Node *node)override;
        virtual void resumeAction(Action *a)override;
        virtual void resumeActionsForNode(Node *node)override;
        virtual void update(float delta);
        std::unordered_set<Ref_ptr<Action>, myhash> actions_;
};
typedef ActionManager_uset ActionManager_impl;
// some function has not yet implemented
void ActionManager_uset::update(float delta)
{
    return ;
    for(auto it=actions_.begin(), end=actions_.end(); it!=end;){
        if(run__(it->get(), delta)){
            actions_.erase(it++);
        }else{
            it++;
        }
    }
}
void ActionManager_uset::addAction(Node *associatedNode, Action *a)
{
    Assert(a->getNode()==nullptr, "Action's node_ is not nullptr when added");
    a->setNode(associatedNode);
    //a->node_ = associatedNode;
    actions_.insert(Ref_ptr<Action>(a));
}
void ActionManager_uset::removeAction(Action *a)
{
    Assert(a->getNode()!=nullptr, "Action's node_ is nullptr when removed");
    actions_.erase(Ref_ptr<Action>(a));
}
void ActionManager_uset::removeActionsForNode(Node *node)
{
    for(auto it=actions_.begin(), end=actions_.end(); it!=end;){
        if((*it)->getNode() == node){
            actions_.erase(it++);
        }else{
            it++;
        }
    }
}
void ActionManager_uset::pauseAction(Action *a){}
void ActionManager_uset::pauseActionsForNode(Node *node){}
void ActionManager_uset::resumeAction(Action *a){}
void ActionManager_uset::resumeActionsForNode(Node *node){}
}
