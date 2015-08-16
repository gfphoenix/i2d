#ifndef NODEVEC_HPP
#define NODEVEC_HPP
#include <vector>
class Node;

class NodeVec<T>
{
public:
    NodeVec():v_(nullptr){}
    inline operator bool()const{return v_!=nullptr;}
private:
    std::vector<T> *v_;
};

#endif // NODEVEC_HPP

