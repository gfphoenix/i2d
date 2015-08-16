#ifndef IDLECONTAINER_HPP
#define IDLECONTAINER_HPP
#include "Ref.hpp"
#include "Scheduler.hpp"
#include "ActionManager.hpp"
#include <vector>
class IdleContainer
{
private:
    Ref_ptr<ActionManager> idleActionManager_;
    Ref_ptr<Scheduler> idleScheduler_;
public:
    IdleContainer();
    ~IdleContainer();

    inline ActionManager *getIdleActionManager()const{return idleActionManager_.get();}
    inline Scheduler *getIdleScheduler()const{return idleScheduler_.get();}
};

#endif // IDLECONTAINER_HPP
