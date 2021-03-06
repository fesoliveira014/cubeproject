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
					RIGHT = 0, TOP,
					BACK, LEFT,
					BOTTOM, FRONT
				};

				FaceName face;
			};

			template<typename Vertex>
			static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<uint>& indices)
			{
				glm::vec3 v1, v2, normal;

				for (uint i = 0; i < vertices.size(); i++) {
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

				for (uint i = 0; i < vertices.size(); i++) {
					vertices[i].m_normal = glm::normalize(vertices[i].m_normal);
				}
			}

			template<typename Vertex>
			static void SetColor(const glm::vec4& color, std::vector<Vertex>& vertices, const std::vector<uint>& indices)
			{
				for (uint i = 0; i < vertices.size(); ++i) {
					vertices[i].m_color = color;
				}
			}

			template<typename Vertex>
			static void AddLine(const glm::vec3& v1, const glm::vec3& v2, std::vector<Vertex>& vertices, std::vector<uint>& indices)
			{
				Vertex pointA, pointB;

				pointA.m_position = v1;
				pointB.m_position = v2;

				uint offset = (uint)vertices.size();
				indices.push_back(offset);
				indices.push_back(offset + 1);

				vertices.push_back(pointA);
				vertices.push_back(pointB);
			}

			template<typename Vertex>
			static void AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				const glm::vec3& v4, std::vector<Vertex>& vertices, std::vector<uint>& indices)
			{
				Vertex cornerA, cornerB, cornerC, cornerD;

				cornerA.m_position = v1;
				cornerB.m_position = v2;
				cornerC.m_position = v3;
				cornerD.m_position = v4;

				uint offset = (uint)vertices.size();
				indices.push_back(offset); indices.push_back(offset + 1); indices.push_back(offset + 2);
				indices.push_back(offset + 2); indices.push_back(offset + 3); indices.push_back(offset);

				vertices.push_back(cornerA);
				vertices.push_back(cornerB);
				vertices.push_back(cornerC);
				vertices.push_back(cornerD);
			}

			template<typename Vertex>
			static void AddQuadUV(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				const glm::vec3& v4, std::vector<Vertex>& vertices, std::vector<uint>& indices)
			{
				Vertex cornerA, cornerB, cornerC, cornerD;

				cornerA.m_position = v1;
				cornerB.m_position = v2;
				cornerC.m_position = v3;
				cornerD.m_position = v4;

				cornerA.m_uv = glm::vec2(0.0f);
				cornerB.m_uv = glm::vec2(1.0f, 0.0f);
				cornerC.m_uv = glm::vec2(1.0f, 1.0f);
				cornerD.m_uv = glm::vec2(0.0f, 1.0f);

				uint offset = (uint)vertices.size();
				indices.push_back(offset); indices.push_back(offset + 1); indices.push_back(offset + 2);
				indices.push_back(offset + 2); indices.push_back(offset + 3); indices.push_back(offset);

				vertices.push_back(cornerA);
				vertices.push_back(cornerB);
				vertices.push_back(cornerC);
				vertices.push_back(cornerD);
			}

			template<typename Vertex>
			static void AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				const glm::vec3& v4, std::vector<Vertex>& vertices, std::vector<uint>& indices,
				const glm::vec4& color)
			{
				Vertex cornerA, cornerB, cornerC, cornerD;

				cornerA.m_position = v1;
				cornerB.m_position = v2;
				cornerC.m_position = v3;
				cornerD.m_position = v4;

				cornerA.m_color = color;
				cornerB.m_color = color;
				cornerC.m_color = color;
				cornerD.m_color = color;

				uint offset = (uint)vertices.size();
				indices.push_back(offset); indices.push_back(offset + 1); indices.push_back(offset + 2);
				indices.push_back(offset + 2); indices.push_back(offset + 3); indices.push_back(offset);

				vertices.push_back(cornerA);
				vertices.push_back(cornerB);
				vertices.push_back(cornerC);
				vertices.push_back(cornerD);
			}

			template<typename Vertex>
			static void AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				const glm::vec3& v4, std::vector<Vertex>& vertices, std::vector<uint>& indices,
				byte type)
			{
				glm::vec4 color;

				switch (type) {

				case 2:
					color = glm::vec4(0.2f, 0.6f, 0.2f, 0.5f);
					break;
				case 3:
					color = glm::vec4(0.901f, 1.0f, 1.0f, 1.0f);
					break;
				case 4:
					color = glm::vec4(0.0, 0.2f, 0.6f, 1.0f);
					break;
				case 5:
					color = glm::vec4(0.2, 0.2f, 0.2f, 1.0f);
					break;
				case 1:
				default:
					color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
					break;
				}

				AddQuad<Vertex>(v1, v2, v3, v4, vertices, indices, color);
			}

			template<typename Vertex>
			static void AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
				std::vector<Vertex>& vertices, std::vector<uint>& indices)
			{
				Vertex a, b, c;

				a.m_position = v1;
				b.m_position = v2;
				c.m_position = v3;

				uint offset = (uint)vertices.size();
				indices.push_back(offset); 
				indices.push_back(offset + 1); 
				indices.push_back(offset + 2);

				vertices.push_back(a);
				vertices.push_back(b);
				vertices.push_back(c);
			}
		}
	}
}

#endif
