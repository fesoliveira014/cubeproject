#ifndef _RAY_H_
#define _RAY_H_

#include "../Common.h"

#include "Math.h"

namespace tactical
{
	namespace math
	{
		class Ray
		{
		public:
			enum RayType
			{
				MMP, MPM, PMM,
				MPP, PMP, PPM,
				MMO, MOM, OMM,
				MOO, OMO, OOM,
				POO, OPO, OOP,
				PPO, POP, OPP,
				MPO, MOP, PMO, 
				POM, OMP, OPM,
				MMM, PPP, OOO
			};

			Ray(const glm::vec3& o, const glm::vec3& d);
			~Ray();

			bool Intersects(AABB& box);

		private:
			Ray();
			void PreCalculate();

		private:
			glm::vec3 m_origin;
			glm::vec3 m_direction;
			glm::vec3 m_inverse;

			RayType m_classification;
			float m_ibyj, m_jbyi, m_kbyj, m_jbyk, m_ibyk, m_kbyi; //slope
			float m_cxy, m_cxz, m_cyx, m_cyz, m_czx, m_czy;
		};
	}
}

#endif