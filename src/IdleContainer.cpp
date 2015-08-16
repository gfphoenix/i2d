#include "IdleContainer.hpp"
#include <algorithm>
#include <vector>
#include <ActionManager.hpp>
#include <Scheduler.hpp>
IdleContainer::IdleContainer()
    :idleActionManager_(ActionManager::create())
    , idleScheduler_(Scheduler::create())
{

}

IdleContainer::~IdleContainer()
{

}
