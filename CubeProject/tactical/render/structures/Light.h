#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		struct Phong
		{
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
		};

		struct Light
		{
			glm::vec3 position;
			glm::vec4 color;

			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;

			Light() 
			{ 
				position = glm::vec3(0); 
				color = glm::vec4(1.0f);

				ambient = glm::vec3(0.8f);
				diffuse = glm::vec3(0.5f);
				specular = glm::vec3(0.2f);
			}

			Light(const glm::vec3& p, const glm::vec4& c, const Phong& phong) 
				: position(p), color(c), ambient(phong.ambient), diffuse(phong.diffuse), specular(phong.specular) {}
		};

		struct DirectionalLight : public Light
		{
			glm::vec3 direction;

			DirectionalLight()
			{
				Light();
				direction = glm::vec3(1.0f);
			}

			DirectionalLight(const glm::vec3& p, const glm::vec4& c, const Phong& phong, const glm::vec3& d) :
				direction(d)
			{
				Light(p, c, phong);
			}

		};

		struct PointLight : public Light
		{
			float constant;
			float linear;
			float quadratic;

			PointLight()
			{
				Light();

				constant = 1.0f;
				linear = 0.09f;
				quadratic = 0.032f;
			}

			PointLight(const glm::vec3& p, const glm::vec4& c, const Phong& phong, const float k, const float l, const float q) :
				constant(k), linear(l), quadratic(q)
			{
				Light(p, c, phong);
			}
		};

		struct SpotLight : public PointLight
		{
			glm::vec3 direction;
			float cutOff;
			float outerCutOff;

			SpotLight()
			{
				PointLight();
				direction = glm::vec3(1.0f);
				cutOff = glm::radians(12.5f);
			}

			SpotLight(const glm::vec3& p, const glm::vec4& c, const Phong& phong, const float k, const float l, const float q, const glm::vec3& d, const float cut, const float outCut)
				: direction(d), cutOff(cut), outerCutOff(outCut)
			{
				PointLight(p, c, phong, k, l, q);
			}
		};
	}
}

#endif