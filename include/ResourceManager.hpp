#ifndef _Resource_Manager_HPP
#define _Resource_Manager_HPP
#include <Ref.hpp>
#include <String.hpp>
#include <unordered_map>
#include <mm.hpp>

class ResourceManager;
class Resource : public Ref
{
    public:
        inline const std::string &getResourceName()const{return name_;}
    protected:
        Resource():Ref(),manager_(nullptr){}
        virtual ~Resource(){}
        virtual void Dispose()=0;
        virtual void Delete()override;
        inline void setResourceManager(ResourceManager *manager){manager_=manager;}
        inline void setResourceName(std::string &&name){name_=std::move(name);}
        inline void setResourceName(const std::string &name){name_=name;}
        friend class ResourceManager;
    private:
        // init is called by resource-manager
        ResourceManager *manager_;
        std::string name_;
};
// resource manager should be a singleton
class ResourceManager : public Ref
{
    public:
        //Make it virtual, so sub-class can do more things
    // return an existing resource or load(sync) and return
        virtual Ref_ptr<Resource> get(const std::string &name);
        bool isLoaded(const std::string &name)const{return map_.find(name)!=map_.cend();}
    protected:
	ResourceManager(){}
        virtual ~ResourceManager(){}
    // do real sync loading work, dirty work
        virtual Ref_ptr<Resource> loadResource(const std::string &name)=0;
        void unlink(const Resource *const res);
        void link(Resource *const res);
        friend class Resource;
        typedef std::unordered_map<std::string, Resource*, std::hash<string>> Map_t;
	Map_t map_;
};
#endif /* _Resource_Manager_HPP */
