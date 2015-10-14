#include <Scheduler.hpp>
#include <Node.hpp>
#include <algorithm>
//#include "impl/Scheduler_umap.inl"
#include "impl/Scheduler_vector.inl"

Ref_ptr<Scheduler> Scheduler::create()
{
    return Ref_ptr<Scheduler>(MM<Scheduler_impl>::New());
}
//void Scheduler::scheduleUpdate(Node *node)
//{schedulePtr(static_cast<void*>(node), "", [node](float dt){return node->update(dt);});}
void Scheduler::test()
{
}
