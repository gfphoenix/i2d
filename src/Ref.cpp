#include <Ref.hpp>
#include <atomic>
#include <debug>
#include <mm.hpp>

void Ref::Retain()
{
    Assert(count_ >= 0, "Ref::Retain count_ <=0");
    count_++;
}
void Ref::Release()
{
    Assert(count_ > 0, "Ref::Release count_ <= 0");
    if(--count_ == 0)
        Delete();
}
void Ref::Delete()
{
    //Debug("Ref::Delete name=<%s> p = %p\n", getInfo().c_str(), this);
    MM<Ref>::Del(this);
}


