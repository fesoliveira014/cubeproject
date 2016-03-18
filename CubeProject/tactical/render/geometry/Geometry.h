#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "../../Common.h"

#include "../structures/Vertex.h"

namespace tactical
{
	namespace render
	{
		namespace geometry
		{
			struct Face
			{
				enum FaceName 
				{
					TOP = 0, BOTTOM,
					RIGHT, LEFT,
					FRONT, BACK
				};

				FaceName face;
			};

			template<typename Vertex>
			static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<uint>& indices)
			{
				glm::vec3 v1, v2, normal;

				for (uint i = 0; i < vertices.size(); i++)
				{
					vertices[i].m_normal = glm::vec3(0.0f);
				}

				for (uint i = 0; i < indices.size() / 3; ++i) {
					uint cornerA = indices[i * 3];
					uint cornerB = indices[i * 3 + 1];
					uint cornerC = indices[i * 3 + 2];

					v1 = vertices[cornerA].m_position - vertices[cornerB].m_position;
					v2 = vertices[cornerC].m_position - vertices[cornerA].m_position;

					normal = glm::cross(v2, v1);

					vertices[cornerA].m_normal += normal;
					vertices[cornerB].m_normal += normal;
					vertices[cornerC].m_normal += normal;
				}

				for (uint i = 0; i < vertices.size(); i++)
				{
					vertices[i].m_normal = glm::normalize(vertices[i].m_normal);
				}
			}

			template<typename Vertex>
			static void AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				const glm::vec3& v4, std::vector<Vertex>& vertices, std::vector<uint>& indices, 
				const glm::vec4& color = (0.0f), bool uv = false)
			{
				Vertex cornerA, cornerB, cornerC, cornerD;

				cornerA.m_position = v1;
				cornerB.m_position = v2;
				cornerC.m_position = v3;
				cornerD.m_position = v4;

				if (color != glm::vec3(0.0f)) {
					cornerA.m_color = color;
					cornerB.m_color = color;
					cornerC.m_color = color;
					cornerD.m_color = color;
				}

				if (uv) {
					cornerA.uv = glm::vec2(1.0f, 1.0f);
					cornerB.uv = glm::vec2(0.0f, 1.0f);
					cornerC.uv = glm::vec2(0.0f, 0.0f);
					cornerD.uv = glm::vec2(1.0f, 0.0f);
				}

				uint offset = (uint)vertices.size();
				indices.push_back(offset); indices.push_back(offset + 1); indices.push_back(offset + 2);
				indices.push_back(offset + 2); indices.push_back(offset + 3); indices.push_back(offset);

				vertices.push_back(cornerA);
				vertices.push_back(cornerB);
				vertices.push_back(cornerC);
				vertices.push_back(cornerD);
			}
		}
	}
}

#endif
