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

	enum class GLTexture : uint
	{
		TEXTURE0 = GL_TEXTURE0, 
		TEXTURE1 = GL_TEXTURE1, 
		TEXTURE2 = GL_TEXTURE2, 
		TEXTURE3 = GL_TEXTURE3, 
		TEXTURE4 = GL_TEXTURE4, 
		TEXTURE5 = GL_TEXTURE5, 
		TEXTURE6 = GL_TEXTURE6, 
		TEXTURE7 = GL_TEXTURE7, 
		TEXTURE8 = GL_TEXTURE8, 
		TEXTURE9 = GL_TEXTURE9, 
		TEXTURE10 = GL_TEXTURE10,
		TEXTURE11 = GL_TEXTURE11,
		TEXTURE12 = GL_TEXTURE12,
		TEXTURE13 = GL_TEXTURE13,
		TEXTURE14 = GL_TEXTURE14,
		TEXTURE15 = GL_TEXTURE15,
		TEXTURE16 = GL_TEXTURE16,
		TEXTURE17 = GL_TEXTURE17,
		TEXTURE18 = GL_TEXTURE18,
		TEXTURE19 = GL_TEXTURE19,
		TEXTURE20 = GL_TEXTURE20,
		TEXTURE21 = GL_TEXTURE21,
		TEXTURE22 = GL_TEXTURE22,
		TEXTURE23 = GL_TEXTURE23,
		TEXTURE24 = GL_TEXTURE24,
		TEXTURE25 = GL_TEXTURE25,
		TEXTURE26 = GL_TEXTURE26,
		TEXTURE27 = GL_TEXTURE27,
		TEXTURE28 = GL_TEXTURE28,
		TEXTURE29 = GL_TEXTURE29,
		TEXTURE30 = GL_TEXTURE30,
		TEXTURE31 = GL_TEXTURE31
	};

	static uint GLTypeSize(GLType type)
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



#endif