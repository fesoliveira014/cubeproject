#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include  "../../Common.h"

namespace tactical 
{
	namespace render
	{
		struct Material
		{
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			float shininess;
		};
	}
}

#endif //_MATERIAL_H_