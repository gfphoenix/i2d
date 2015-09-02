#ifndef _ACTION_MANAGER_HPP
#define _ACTION_MANAGER_HPP
#include "Ref.hpp"
#include "Action.hpp"
class Node;
class Scene;
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
        virtual void update(float delta)=0;
        static Ref_ptr<ActionManager> create();
    protected:
        friend class Scene;
        friend class Node;
        virtual void moveNode(ActionManager *am, Scene *scene)=0;
        inline bool run__(Action *a, float dt)const{a->step(dt);a->update();return a->isDone();}
        virtual ~ActionManager(){}
};
#endif
