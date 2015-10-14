#ifndef _NOODLE_TYPES
#define _NOODLE_TYPES
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat4x3.hpp"
#include "glm/mat4x4.hpp"

#include <stdint.h>
#include <string>
#include <vector>

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

typedef glm::mat2 Mat2;
typedef glm::mat3 Mat3;
typedef glm::mat4 Mat4;

typedef glm::mat3x4 Mat3x4;
typedef glm::mat4x3 Mat4x3;

typedef glm::ivec2 iVec2;

typedef glm::vec4 Color4;
typedef glm::vec3 Color3;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef unsigned int uint;

struct Rect
{
    Vec2 oxy;
    Vec2 size;
    inline float x()const{return oxy.x;}
    inline float y()const{return oxy.y;}
    inline float width()const{return size.x;}
    inline float height()const{return size.y;}
    inline void setOrigin(float x, float y){oxy.x=x;oxy.y=y;}
    inline void setSize(float w, float h){size.x=w;size.y=h;}
    inline void setOrigin(const Vec2 &OXY){oxy=OXY;}
    inline void setSize(const Vec2 &SIZE){size=SIZE;}
};
struct iRect
{
    iVec2 oxy;
    iVec2 size;
    inline int x()const{return oxy.x;}
    inline int y()const{return oxy.y;}
    inline int width()const{return size.x;}
    inline int height()const{return size.y;}
    inline void setOrigin(int x, int y){oxy.x=x;oxy.y=y;}
    inline void setSize(int w, int h){size.x=w;size.y=h;}
    inline void setOrigin(const iVec2 &OXY){oxy=OXY;}
    inline void setSize(const iVec2 &SIZE){size=SIZE;}
};

#endif // _NOODLE_TYPES
