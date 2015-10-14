#ifndef _VERTEX_UV_COLOR_HPP
#define _VERTEX_UV_COLOR_HPP
#include "types.hpp"
struct V2F_T2F_C4F
{
    Vec2 v;
    Vec2 t;
    Vec4 c;
};

struct V3F_T2F_C3F
{
    Vec3 v;
    Vec2 t;
    Vec3 c;
};
struct V3F_T2F_C4F
{
    Vec3 v;
    Vec2 t;
    Vec4 c;
};


struct V324_Quad
{
    V3F_T2F_C4F bl;
    V3F_T2F_C4F br;
    V3F_T2F_C4F tl;
    V3F_T2F_C4F tr;
};
struct V224_Quad
{
    //union
    //{
	//struct
	//{
            V2F_T2F_C4F bl;
            V2F_T2F_C4F br;
            V2F_T2F_C4F tl;
            V2F_T2F_C4F tr;
	//};
	//V2F_T2F_C4F vtc[4];
    //};
    //V224_Quad(){}
    //~V224_Quad(){}
};


































































#endif
