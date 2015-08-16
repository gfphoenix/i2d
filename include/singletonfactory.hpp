#ifndef SINGLETONFACTORY_HPP
#define SINGLETONFACTORY_HPP
#include <Ref.hpp>
#include <IdleContainer.hpp>
#include <Director.hpp>
#include <Scheduler.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>
class SingletonFactory
{
public:
    SingletonFactory();
    ~SingletonFactory();
    template <typename tp_>
    inline static tp_ *getInstance(){
	static Ref_ptr<tp_> _tmd_obj_;
	if(_tmd_obj_ == nullptr){
	    _tmd_obj_ = MM<tp_>::New();
	}
	return _tmd_obj_.get();
    }
};

#endif // SINGLETONFACTORY_HPP
