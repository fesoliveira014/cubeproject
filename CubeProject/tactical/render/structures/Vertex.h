#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		struct Vertex3f
		{
			glm::vec3 m_position;

			Vertex3f() { }
			Vertex3f(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec4 color)
				: m_position(position)
			{

			}
		};

		struct Vertex3f2f
		{
			glm::vec3 m_position;
			glm::vec2 m_uv;

			Vertex3f2f() { }
			Vertex3f2f(glm::vec3 position, glm::vec2 uv)
				: m_position(position),  m_uv(uv)
			{

			}
		};

		struct Vertex3f3f
		{
			glm::vec3 m_position;
			glm::vec3 m_normal;

			Vertex3f3f() { }
			Vertex3f3f(glm::vec3 position, glm::vec3 normal)
				: m_position(position), m_normal(normal)
			{

			}
		};

		struct Vertex3f4f
		{
			glm::vec3 m_position;
			glm::vec4 m_color;

			Vertex3f4f() { }
			Vertex3f4f(glm::vec3 position, glm::vec4 color)
				: m_position(position), m_color(color)
			{

			}
		};

		struct Vertex3f3f2f
		{
			glm::vec3 m_position;
			glm::vec3 m_normal;
			glm::vec2 m_uv;

			Vertex3f3f2f() { }
			Vertex3f3f2f(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
				: m_position(position), m_normal(normal), m_uv(uv)
			{

			}
		};

		struct Vertex3f2f4f
		{
			glm::vec3 m_position;
			glm::vec2 m_uv;
			glm::vec4 m_color;

			Vertex3f2f4f() { }
			Vertex3f2f4f(glm::vec3 position, glm::vec2 uv, glm::vec4 color)
				: m_position(position), m_uv(uv), m_color(color)
			{

			}
		};

		struct Vertex3f3f2f4f
		{
			glm::vec3 m_position;
			glm::vec3 m_normal;
			glm::vec2 m_uv;
			glm::vec4 m_color;

			Vertex3f3f2f4f() { }
			Vertex3f3f2f4f(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec4 color)
				: m_position(position), m_normal(normal), m_uv(uv), m_color(color)
			{

			}
		};
	}
}

#endif