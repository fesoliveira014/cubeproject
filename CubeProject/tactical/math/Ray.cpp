#include "Ray.h"

namespace tactical
{
	namespace math
	{
		Ray::Ray() : m_origin(glm::vec3(0.0f)), m_direction(glm::vec3(0.0f, 0.0f, 1.0f)) 
		{
			PreCalculate();
		}

		Ray::Ray(const glm::vec3& o, const glm::vec3& d) : m_origin(o), m_direction(d) 
		{
			PreCalculate();
		}

		void Ray::PreCalculate() {
			m_inverse = 1.0f / m_direction;

			m_ibyj = m_direction.x * m_inverse.y;
			m_ibyk = m_direction.x * m_inverse.z;
			m_jbyi = m_direction.y * m_inverse.x;
			m_jbyk = m_direction.y * m_inverse.z;
			m_kbyj = m_direction.z * m_inverse.y;
			m_kbyi = m_direction.z * m_inverse.x;

			m_cxy = m_origin.y - m_jbyi * m_origin.x;
			m_cxz = m_origin.z - m_kbyi * m_origin.x;
			m_cyx = m_origin.x - m_ibyj * m_origin.y;
			m_cyz = m_origin.z - m_kbyj * m_origin.y;
			m_czx = m_origin.x - m_ibyk * m_origin.z;
			m_czy = m_origin.y - m_jbyk * m_origin.z;

			if (m_direction.x < 0) {
				if (m_direction.y < 0) {
					if (m_direction.z < 0) m_classification = RayType::MMM;
					else if (m_direction.z > 0) m_classification = RayType::MMP;
					else m_classification = RayType::MMO;
				}
				else {
					if (m_direction.z < 0) {
						if (m_direction.y == 0) m_classification = RayType::MOM;
						else m_classification = RayType::MPM;
					}
					else {
						if ((m_direction.y == 0) && (m_direction.z == 0)) m_classification = RayType::MOO;
						else if (m_direction.z == 0) m_classification = RayType::MPO;
						else if (m_direction.y == 0) m_classification = RayType::MOP;
						else m_classification = RayType::MPP;
					}
				}
			}
			else {
				if (m_direction.y < 0) {
					if (m_direction.z < 0) {
						if (m_direction.x == 0) m_classification = RayType::OMM;
						else m_classification = RayType::PMM;
					}
					else {
						if ((m_direction.x == 0) && (m_direction.z == 0)) m_classification = RayType::OMO;
						else if (m_direction.x == 0) m_classification = RayType::OMP;
						else if (m_direction.z == 0) m_classification = RayType::PMO;
						else m_classification = RayType::PMP;
					}
				}
				else {
					if (m_direction.z < 0) {
						if ((m_direction.x == 0) && (m_direction.y == 0)) m_classification = RayType::OOM;
						else if (m_direction.x == 0) m_classification = RayType::OPM;
						else if (m_direction.y == 0) m_classification = RayType::POM;
						else m_classification = RayType::PPM;
					}
					else {
						if (m_direction.x == 0) {
							//if ((m_direction.y == 0) && (m_direction.z == 0)) m_classification = RayType::OOO;
							if (m_direction.y == 0) m_classification = RayType::OOP;
							else if (m_direction.z == 0) m_classification = RayType::OPO;
							else m_classification = RayType::OPP;
						}
						else {
							if ((m_direction.y == 0) && (m_direction.z == 0)) m_classification = RayType::POO;
							else if (m_direction.y == 0) m_classification = RayType::POP;
							else if (m_direction.z == 0) m_classification = RayType::PPO;
							else m_classification = RayType::PPP;
						}
					}
				}
			}	
		}

		
		/**
			Intersection of a ray and a aligned-axis bounding box.

			Implemented using the method described on "Fast Ray/Axis-Aligned Bounding Box
			Overlap Tests using Ray Slopes", by Martin Eisemann et al.

			Avoids division by zero in some cases and accelerate intersection calculations
			by precalculating many characteristics of the ray.

			@params box The aligned-axis bouding box.
			@see AABB
			@return True if there is an intersection and false otherwise.
		*/

		bool Ray::Intersects(AABB& box)
		{
			switch (m_classification)
			{
			case RayType::MMM:
				if ((m_origin.x < box.GetMin().x) || (m_origin.y < box.GetMin().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyi * box.GetMin().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMax().x + m_cyx > 0)
					|| (m_jbyk * box.GetMin().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMax().z + m_cyz > 0)
					|| (m_kbyi * box.GetMin().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::MMP:
				if ((m_origin.x < box.GetMin().x) || (m_origin.y < box.GetMin().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyi * box.GetMin().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMax().x + m_cyx > 0)
					|| (m_jbyk * box.GetMax().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMin().z + m_cyz < 0)
					|| (m_kbyi * box.GetMin().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::MPM:
				if ((m_origin.x < box.GetMin().x) || (m_origin.y > box.GetMax().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyi * box.GetMin().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMax().x + m_cyx > 0)
					|| (m_jbyk * box.GetMin().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMax().z + m_cyz > 0)
					|| (m_kbyi * box.GetMin().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::MPP:
				if ((m_origin.x < box.GetMin().x) || (m_origin.y > box.GetMax().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyi * box.GetMin().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMax().x + m_cyx > 0)
					|| (m_jbyk * box.GetMax().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMin().z + m_cyz < 0)
					|| (m_kbyi * box.GetMin().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::PMM:
				if ((m_origin.x > box.GetMax().x) || (m_origin.y < box.GetMin().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyi * box.GetMax().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMin().x + m_cyx < 0)
					|| (m_jbyk * box.GetMin().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMax().z + m_cyz > 0)
					|| (m_kbyi * box.GetMax().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::PMP:
				if ((m_origin.x > box.GetMax().x) || (m_origin.y < box.GetMin().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyi * box.GetMax().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMin().x + m_cyx < 0)
					|| (m_jbyk * box.GetMax().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMin().z + m_cyz < 0)
					|| (m_kbyi * box.GetMax().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::PPM:
				if ((m_origin.x > box.GetMax().x) || (m_origin.y > box.GetMax().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyi * box.GetMax().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMin().x + m_cyx < 0)
					|| (m_jbyk * box.GetMin().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMax().z + m_cyz > 0)
					|| (m_kbyi * box.GetMax().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::PPP:
				if ((m_origin.x > box.GetMax().x) || (m_origin.y > box.GetMax().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyi * box.GetMax().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMin().x + m_cyx < 0)
					|| (m_jbyk * box.GetMax().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMin().z + m_cyz < 0)
					|| (m_kbyi * box.GetMax().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::OMM:
				if ((m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyk * box.GetMin().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMax().z + m_cyz > 0))
					return false;
				return true;
			case RayType::OMP:
				if ((m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyk * box.GetMax().z - box.GetMax().y + m_czy > 0)
					|| (m_kbyj * box.GetMin().y - box.GetMin().z + m_cyz < 0))
					return false;
				return true;
			case RayType::OPM:
				if ((m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y > box.GetMax().y) || (m_origin.z < box.GetMin().z)
					|| (m_jbyk * box.GetMin().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMax().z + m_cyz > 0))
					return false;
				return true;
			case RayType::OPP:
				if ((m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y > box.GetMax().y) || (m_origin.z > box.GetMax().z)
					|| (m_jbyk * box.GetMax().z - box.GetMin().y + m_czy < 0)
					|| (m_kbyj * box.GetMax().y - box.GetMin().z + m_cyz < 0))
					return false;
				return true;
			case RayType::MOM:
				if ((m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.x < box.GetMin().x) || (m_origin.z < box.GetMin().z)
					|| (m_kbyi * box.GetMin().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::MOP:
				if ((m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.x < box.GetMin().x) || (m_origin.z > box.GetMax().z)
					|| (m_kbyi * box.GetMin().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMax().x + m_czx > 0))
					return false;
				return true;
			case RayType::POM:
				if ((m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.x > box.GetMax().x) || (m_origin.z < box.GetMin().z)
					|| (m_kbyi * box.GetMax().x - box.GetMax().z + m_cxz > 0)
					|| (m_ibyk * box.GetMin().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::POP:
				if ((m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.x > box.GetMax().x) || (m_origin.z > box.GetMax().z)
					|| (m_kbyi * box.GetMax().x - box.GetMin().z + m_cxz < 0)
					|| (m_ibyk * box.GetMax().z - box.GetMin().x + m_czx < 0))
					return false;
				return true;
			case RayType::MMO:
				if ((m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.y < box.GetMin().y)
					|| (m_jbyi * box.GetMin().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMax().x + m_cyx > 0))
					return false;
				return true;
			case RayType::MPO:
				if ((m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.y > box.GetMax().y)
					|| (m_jbyi * box.GetMin().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMax().x + m_cyx > 0))
					return false;
				return true;
			case RayType::PMO:
				if ((m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z)
					|| (m_origin.x > box.GetMax().x) || (m_origin.y < box.GetMin().y)
					|| (m_jbyi * box.GetMax().x - box.GetMax().y + m_cxy > 0)
					|| (m_ibyj * box.GetMin().y - box.GetMin().x + m_cyx < 0))
					return false;
				return true;
			case RayType::PPO:
				if ((m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z)
					|| (m_origin.x > box.GetMax().x) || (m_origin.y > box.GetMax().y)
					|| (m_jbyi * box.GetMax().x - box.GetMin().y + m_cxy < 0)
					|| (m_ibyj * box.GetMax().y - box.GetMin().x + m_cyx < 0))
					return false;
				return true;
			case RayType::MOO:
				if ((m_origin.x < box.GetMin().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z))
					return false;
				return true;
			case RayType::POO:
				if ((m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y)
					|| (m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z))
					return false;
				return true;
			case RayType::OMO:
				if ((m_origin.y < box.GetMin().y)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z))
					return false;
				if ((m_origin.y > box.GetMax().y)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z))
					return false;
				if ((m_origin.z < box.GetMin().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				if ((m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				return true;
			case RayType::OPO:
				if ((m_origin.y > box.GetMax().y)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.z < box.GetMin().z) || (m_origin.z > box.GetMax().z))
					return false;
				if ((m_origin.z < box.GetMin().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				if ((m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				return true;
			case RayType::OOM:
				if ((m_origin.z < box.GetMin().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				if ((m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				return true;
			case RayType::OOP:
				if ((m_origin.z > box.GetMax().z)
					|| (m_origin.x < box.GetMin().x) || (m_origin.x > box.GetMax().x)
					|| (m_origin.y < box.GetMin().y) || (m_origin.y > box.GetMax().y))
					return false;
				return true;
			}
			return false;
		}
	}
}