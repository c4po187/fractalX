#ifndef __VERTEX_H__
#define __VERTEX_H__ 1

#include <glm/glm.hpp>
#include <cstddef> // offsetof

using namespace glm;

struct vertex
{
	vec3 pos;
	vec2 uv;
};

#endif /* __VERTEX_H__ */
