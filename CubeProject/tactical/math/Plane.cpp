#include "Plane.h"

namespace tactical
{
	namespace math
	{
		Plane::Plane()
		{
			m_normal = glm::vec3(0.0f);
			m_scalar = 0.0f;
		}

		Plane::Plane(const Plane& plane)
		{
			m_normal = plane.m_normal;
			m_scalar = plane.m_scalar;
		}

		Plane::Plane(const glm::vec3& normal, float scalar) : m_normal(normal), m_scalar(scalar)
		{

		}

		Plane::Plane(float x, float y, float z, float scalar)
		{
			m_normal = glm::vec3(x, y, z);
			m_scalar = scalar;
		}

		Plane::Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		{
			glm::vec3 v1 = b - a;
			glm::vec3 v2 = c - a;

			m_normal = glm::normalize(glm::cross(v1, v2));
			m_scalar = glm::dot(-m_normal, a);
		}

		Plane::~Plane()
		{

		}

		float Plane::DistanceFromPoint(const glm::vec3& point)
		{
			return glm::dot(m_normal, point) + m_scalar;
		}

		Plane::PlaneSide Plane::Intersects(const AABB& box)
		{
			float distance = DistanceFromPoint(box.GetCenter());
			float extent = glm::abs(glm::dot(m_normal, box.GetSize()));
			if (distance - extent > 0) return PlaneSide::OUTSIDE;
			else if (distance + extent < 0) return PlaneSide::INSIDE;
			return PlaneSide::INTERSECTING;
		}

		void Plane::Set(const glm::vec3& normal, float scalar)
		{
			m_normal = normal;
			m_scalar = scalar;
		}

		void Plane::SetNormal(const glm::vec3& normal)
		{
			m_normal = normal;
		}

		void Plane::SetScalar(float scalar)
		{
			m_scalar = scalar;
		}
	}
}
