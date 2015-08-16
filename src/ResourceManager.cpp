#include <debug>
#include <ResourceManager.hpp>

// 1. 释放非内存资源（不含内存，如纹理，文件，。。。
// 2. 解除ResourceManager对Resource的引用
// 3. 释放内存资源
void Resource::Delete()
{
    Dispose();
    // remove reference from resource manager
    if(manager_ != nullptr)
        manager_->unlink(this);
    manager_ = nullptr;
    // must put Ref::Delete() last
    // No more reference to `this'
    Ref::Delete();
}

Ref_ptr<Resource> ResourceManager::get(const string &name)
{
    auto x = map_.find(name);
    if(x != map_.cend())
        return Ref_ptr<Resource>(x->second);
    auto res = loadResource(name);
    if(res){
        link(res.get());
        res->setResourceManager(this);
        res->setName(name);
    }
    return res;
}

void ResourceManager::link(Resource *const res)
{
    auto const &name = res->getName();
    auto x = map_.find(name);
    Assert(x==map_.cend(), "ResourceManager::link to an existed Resource");
    map_[name] = res;
}
void ResourceManager::unlink(const Resource *const res)
{
    auto x = map_.find(res->getName());
    if(x != map_.cend())
        map_.erase(x);
}









