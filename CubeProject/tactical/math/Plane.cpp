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
			float d = glm::dot(box.GetCenter(), m_normal);
			float r = box.GetSize().x * abs(m_normal.x) + box.GetSize().y * abs(m_normal.y) + box.GetSize().z * abs(m_normal.z);
			float dpr = d + r;

			if (dpr < -m_scalar)	return PlaneSide::OUTSIDE;

			return PlaneSide::INSIDE;
		}

		void Plane::Set(const glm::vec3& normal, float scalar)
		{
			m_normal = normal;
			m_scalar = scalar;
		}

		void Plane::SetAndNormalize(const glm::vec3& normal, float scalar)
		{
			float length = glm::length(normal);

			m_normal = normal / length;
			m_scalar = scalar / length;
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
