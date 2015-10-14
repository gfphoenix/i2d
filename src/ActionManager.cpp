#include <ActionManager.hpp>
#include <mm.hpp>
#include "impl/ActionManager_vector.inl"

Ref_ptr<ActionManager> ActionManager::create()
{
    return MM<ActionManager_impl>::New();
}
