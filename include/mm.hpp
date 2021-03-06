#ifndef MM_HPP_
#define MM_HPP_
// need for malloc & free
#include <stdlib.h>
#include <utility>
struct M
{
    static inline void *Malloc(size_t n){return malloc(n);}
    static inline void Free(void *ptr){return free(ptr);}
};
template <typename tp>
class MM
{
public:
    static inline void * Malloc(){return M::Malloc(sizeof(tp));}
    static inline void Free(const tp *ptr){M::Free((void*)ptr);}
    static inline void Del(tp *ptr){if(ptr){ptr->~tp();Free(ptr);}}
    static inline tp * New(){return new(Malloc())tp();}
    template <typename ... _Args>
    static inline tp * New(_Args&&...args)
    {return new(Malloc())tp(std::forward<_Args>(args)...);}
};


#define DECLARE_PLACEMENT   \
	inline void *operator new(size_t, void *p){return p;} \
	inline void operator delete(void *){} \
    template <typename _R_a_n__D___m_____> friend class MM;

#endif // MM_HPP_

