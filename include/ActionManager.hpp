#ifndef _ACTION_MANAGER_HPP
#define _ACTION_MANAGER_HPP
#include "Ref.hpp"
#include "Action.hpp"
class Node;
class ActionManager : public Ref
{
    public:
        virtual const std::string getInfo()const{return std::string("ActionManager-0 ");}
        virtual void addAction(Node *associatedNode, Action *a, bool pause=false)=0;
        virtual void removeAction(Action *a)=0;
        virtual void removeActionsForNode(Node *node)=0;

        virtual void pauseAction(Action *a)=0;
        virtual void pauseActionsForNode(Node *node)=0;
        virtual void resumeAction(Action *a)=0;
        virtual void resumeActionsForNode(Node *node)=0;
        virtual void moveNode(ActionManager *am, Node *node)=0;
        virtual void update(float delta)=0;
        static Ref_ptr<ActionManager> create();
    protected:
//        inline void setNode__(Action *a, Node *n)const{a->node_=n;}
        inline bool run__(Action *a, float dt)const{a->step(dt);a->update();return a->isDone();}
        virtual ~ActionManager(){}
};
#endif
