#ifndef _LAYER_HPP_
#define _LAYER_HPP_
#include <Node.hpp>

class Layer : public Node
{
    public:
    // Need to init layer size to window size
    Layer():Node(){setAnchor(0,0);}
};
#endif
