#ifndef _TYPES_H_
#define _TYPES_H_

#include "glew\glew.h"

namespace tactical
{
	typedef unsigned int uint;
	typedef uint8_t byte;

	enum class GLType
	{
		FLOAT  = GL_FLOAT,
		INT    = GL_INT,
		UINT   = GL_UNSIGNED_INT,
		BYTE   = GL_BYTE,
		UBYTE  = GL_UNSIGNED_BYTE,
		SHORT  = GL_SHORT,
		USHORT = GL_UNSIGNED_SHORT,

		TRUE   = GL_TRUE,
		FALSE  = GL_FALSE
	};

	namespace 
	{
		uint GLTypeSize(GLType type)
		{
			switch (type)
			{
			case GLType::UBYTE:
			case GLType::BYTE:
				return sizeof(GLbyte);
				break;

			case GLType::UINT:
			case GLType::INT:
				return sizeof(GLint);
				break;

			case GLType::USHORT:
			case GLType::SHORT:
				return sizeof(GLshort);
				break;

			case GLType::FLOAT:
				return sizeof(GLfloat);
			}

			return 0;
		}
	}
}



#endif