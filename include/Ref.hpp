#ifndef _REF_HPP_
#define _REF_HPP_
#include "config.hpp"
#include "mm.hpp"
#include <atomic>
#include <string>

template <typename T>
class Ref_ptr;
class Ref;

class Ref
{
    // contains an int, 
    private:
    // maybe a plain int
        std::atomic<int> count_;
    public:
        Ref():count_(0){}
        virtual ~Ref(){}
        virtual void Delete(); // when count_ down to 0
        virtual const std::string getInfo()const{return "Ref";}
        void Retain();
        void Release();
        inline int UseCount()const{return std::atomic_load<int>(&count_);}
    private:
        template <typename T>
        friend class Ref_ptr;
protected:
    DECLARE_PLACEMENT
};

// Ref_ptr
template <typename Tp_>
class Ref_ptr final
{
    private:
        Tp_ *pointer_;
    public:
            inline Ref_ptr(Tp_ *p=nullptr):pointer_(p)
            {
                if(p != nullptr){
                    Assert(p->UseCount()>=0, "Init pointer should set use-count to >=0");
                    p->count_++;
                }
            }
            inline Ref_ptr(const Ref_ptr &ptr):pointer_(ptr.pointer_)
            {
                if(pointer_ != nullptr)
                    pointer_->Retain();
            }
            inline Ref_ptr(Ref_ptr &&ptr):pointer_(ptr.pointer_)
            {
                ptr.pointer_ = nullptr;
            }
            inline ~Ref_ptr()
            {
                if(pointer_ != nullptr)
                    pointer_->Release();
            }
            inline Ref_ptr &operator=(const Ref_ptr &ptr)
            {
                pointer_ = ptr.pointer_;
                if(pointer_ != nullptr)
                    pointer_->Retain();
                return *this;
            }
            inline Ref_ptr &operator=(Ref_ptr &&ptr)
            {
                pointer_ = ptr.pointer_;
                ptr.pointer_ = nullptr;
                return *this;
            }
            inline Tp_* operator->()const{return pointer_;}
            inline Tp_& operator*()const{return *pointer_;}
            inline bool operator==(const Ref_ptr &p)const{return pointer_==p.pointer_;}
            inline bool operator==(const Ref *const p)const{return pointer_==p;}
            inline bool operator!=(const Ref_ptr &p)const{return pointer_!=p.pointer_;}
            inline bool operator!=(const Ref *const p)const{return pointer_!=p;}
            inline operator bool()const{return pointer_ != nullptr;}
            inline Tp_ *get()const{return pointer_;}
            inline void reset()
            {
                if(pointer_ != nullptr){
                    pointer_->Release();
                    pointer_ = nullptr;
                }
            }
    private:
            // no heap var any more
            FORCE_STACK_CLASS
};
#endif
