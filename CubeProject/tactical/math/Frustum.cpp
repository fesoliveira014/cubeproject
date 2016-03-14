#include "Frustum.h"

namespace tactical
{
	namespace math
	{
		Frustum::Frustum()
		{

		}

		Frustum::~Frustum()
		{

		}

		void Frustum::Update(glm::mat4 clipMatrix)
		{
			// need to fix this, transposing every frame is costly
			glm::mat4 matrix = glm::transpose(clipMatrix);

			m_planes[0].Set(glm::vec3(matrix[0].w - matrix[0].x, matrix[1].w - matrix[1].x, matrix[2].w - matrix[2].x), matrix[3].w - matrix[3].x);
			m_planes[1].Set(glm::vec3(matrix[0].w + matrix[0].x, matrix[1].w + matrix[1].x, matrix[2].w + matrix[2].x), matrix[3].w + matrix[3].x);
			m_planes[2].Set(glm::vec3(matrix[0].w + matrix[0].y, matrix[1].w + matrix[1].y, matrix[2].w + matrix[2].y), matrix[3].w + matrix[3].y);
			m_planes[3].Set(glm::vec3(matrix[0].w - matrix[0].y, matrix[1].w - matrix[1].y, matrix[2].w - matrix[2].y), matrix[3].w - matrix[3].y);
			m_planes[4].Set(glm::vec3(matrix[0].w - matrix[0].z, matrix[1].w - matrix[1].z, matrix[2].w - matrix[2].z), matrix[3].w - matrix[3].z);
			m_planes[5].Set(glm::vec3(matrix[0].w + matrix[0].z, matrix[1].w + matrix[1].z, matrix[2].w + matrix[2].z), matrix[3].w + matrix[3].z);
		}

		void Frustum::Update(glm::mat4 projection, glm::mat4 modelView)
		{
			Update(projection * modelView);
		}

		bool Frustum::Contains(const glm::vec3& point)
		{
			for (int i = 0; i < 6; i++) {
				if (m_planes[i].DistanceFromPoint(point) < 0)
					return false;
			}
			return true;
		}

		bool Frustum::Contains(const Sphere& sphere)
		{
			float distance;
			
			for (int i = 0; i < 6; i++) {
				distance = m_planes[i].DistanceFromPoint(sphere.center);
				if (distance < -sphere.radius)
					return false;
				else if (distance < sphere.radius)
					return true;
			}
			return true;
		}

		bool Frustum::Contains(const AABB& box)
		{
			for (int i = 0; i < 6; ++i) {
				if (m_planes[i].Intersects(box) == Plane::PlaneSide::OUTSIDE)
					return false;
			}
			return true;
		}
	}
}