/*
 *  glslshader.h
 *  glgl_mix.linux
 *
 *  Created by Raphaël Lemoine on 11/15/11.
 *  Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>

/*----------------------------------------------------------------------------*/

#if (defined NDEBUG)

#define	GL_ASSERT(a)		a;

#else

#define	GL_ASSERT(a)														\
{																			\
	GLenum	_error;															\
																			\
	a;																		\
	_error = glGetError();													\
	if (_error != 0)														\
	{																		\
		fprintf(stdout, #a ": error 0x%04X\n", _error);						\
		fflush(stdout);														\
		assert(0);															\
	}																		\
}

#endif

/*----------------------------------------------------------------------------*/

GLhandleARB	shader_create(const char* vertex, const char* fragment);
void		shader_delete(GLhandleARB);

/** EOF ***********************************************************************/
