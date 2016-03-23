#ifndef _RAY_H_
#define _RAY_H_

#include "../Common.h"

#include "AABB.h"

namespace tactical
{
	namespace math
	{
		struct RayCastResult
		{
			bool hit;
			float length;
			glm::vec3 pos;
			glm::vec3 oldPos;

			RayCastResult() : hit(false), length(0.0f), pos(glm::vec3(0)), oldPos(glm::vec3(0))
			{

			}

			RayCastResult(const RayCastResult& r)
			{
				hit = r.hit;
				length = r.length;
				pos = r.pos;
				oldPos = r.oldPos;
			}
		};

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

			Ray();
			Ray(const glm::vec3& o, const glm::vec3& d);
			Ray(const Ray& rhs) = default;
			Ray(Ray&&) = default;
			~Ray() = default;

			bool Intersects(AABB& box);

			inline glm::vec3 GetDirection() const { return m_direction; }
			inline glm::vec3 GetInverse() const { return m_inverse; }
			inline glm::vec3 GetOrigin() const { return m_origin; }

			Ray& operator=(const Ray&) = default;
			Ray& operator=(Ray&&) = default;

		private:
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