#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		struct Light
		{
			glm::vec3 position;
			glm::vec4 colour;
		
			Light() 
			{ 
				position = glm::vec3(0); 
				colour = glm::vec4(1.0f);
			}

			Light(const glm::vec3& p, const glm::vec4& c) : position(p), colour(c) {}
		};

		struct DirectionalLight : public Light
		{
			glm::vec3 direction;

			DirectionalLight()
			{
				Light();
				direction = glm::vec3(1.0f);
			}

			DirectionalLight(const glm::vec3& p, const glm::vec4& c, const glm::vec3& d) :
				direction(d)
			{
				Light(p, c);
			}

		};

		struct PointLight : public Light
		{
			float attenuationConstant;
			float attenuationLinear;
			float attenuationQuadratic;

			PointLight()
			{
				Light();

				attenuationConstant = 1.0f;
				attenuationLinear = 0.09f;
				attenuationQuadratic = 0.032f;
			}

			PointLight(const glm::vec3& p, const glm::vec4& c, const float k, const float l, const float q) :
				attenuationConstant(k), attenuationLinear(l), attenuationQuadratic(q)
			{
				Light(p, c);
			}
		};

		struct SpotLight : public Light
		{
			glm::vec3 direction;
			float cutOff;

			SpotLight()
			{
				Light();
				direction = glm::vec3(1.0f);
				cutOff = glm::radians(12.5f);
			}

			SpotLight(const glm::vec3& p, const glm::vec4& c, const glm::vec3& d, const float cut)
				: direction(d), cutOff(cut)
			{
				Light(p, c);
			}
		};
	}
}

#endif