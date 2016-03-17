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
			m_planes[0].SetAndNormalize(glm::vec3(clipMatrix[0].w + clipMatrix[0].x, clipMatrix[1].w + clipMatrix[1].x, clipMatrix[2].w + clipMatrix[2].x), clipMatrix[3].w + clipMatrix[3].x); // right
			m_planes[1].SetAndNormalize(glm::vec3(clipMatrix[0].w - clipMatrix[0].x, clipMatrix[1].w - clipMatrix[1].x, clipMatrix[2].w - clipMatrix[2].x), clipMatrix[3].w - clipMatrix[3].x); // left
			m_planes[2].SetAndNormalize(glm::vec3(clipMatrix[0].w + clipMatrix[0].y, clipMatrix[1].w + clipMatrix[1].y, clipMatrix[2].w + clipMatrix[2].y), clipMatrix[3].w + clipMatrix[3].y); // top
			m_planes[3].SetAndNormalize(glm::vec3(clipMatrix[0].w - clipMatrix[0].y, clipMatrix[1].w - clipMatrix[1].y, clipMatrix[2].w - clipMatrix[2].y), clipMatrix[3].w - clipMatrix[3].y); // bottom
			m_planes[4].SetAndNormalize(glm::vec3(clipMatrix[0].w + clipMatrix[0].z, clipMatrix[1].w + clipMatrix[1].z, clipMatrix[2].w + clipMatrix[2].z), clipMatrix[3].w + clipMatrix[3].z); // front
			m_planes[5].SetAndNormalize(glm::vec3(clipMatrix[0].w - clipMatrix[0].z, clipMatrix[1].w - clipMatrix[1].z, clipMatrix[2].w - clipMatrix[2].z), clipMatrix[3].w - clipMatrix[3].z); // back
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