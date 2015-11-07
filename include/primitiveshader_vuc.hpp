#ifndef PRIMITIVESHADER_PUC
#define PRIMITIVESHADER_PUC
#include "types.hpp"
#include "PrimitiveShader.hpp"
#include <vector>
// geometry shader :
// position + color(uniform)
class PrimitiveShader_PuC : public PrimitiveShader
{

protected:
    std::vector<Vec2> pos_;
};

#endif // PRIMITIVESHADER_PUC

