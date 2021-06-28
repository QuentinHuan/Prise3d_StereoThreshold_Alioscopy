/*
 *  glsl_shader.cpp
 *  ps_mix
 *
 *  Created by Raphaël Lemoine on 1/30/12.
 *  Copyright 2012 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#include "main.h"
#include "glslshader.h"

static GLhandleARB shader_compile(GLenum type, const char* shader);

/******************************************************************************/

GLhandleARB shader_create(const char* vertex, const char* fragment)
{
	GLhandleARB	vertexShader;
	GLhandleARB	fragmentShader;
	GLhandleARB	programObject;

	vertexShader = shader_compile(GL_VERTEX_SHADER_ARB, vertex);
	assert(vertexShader);

	fragmentShader = shader_compile(GL_FRAGMENT_SHADER_ARB, fragment);
	assert(fragmentShader);

	programObject = glCreateProgramObjectARB();
	assert(programObject);
	GL_ASSERT(glAttachObjectARB(programObject, vertexShader));
	GL_ASSERT(glDeleteObjectARB(vertexShader));
	GL_ASSERT(glAttachObjectARB(programObject, fragmentShader));
	GL_ASSERT(glDeleteObjectARB(fragmentShader));
	GL_ASSERT(glLinkProgramARB(programObject));

	GLint	linked = 0;
	GL_ASSERT(glGetObjectParameterivARB(programObject, GL_OBJECT_LINK_STATUS_ARB, &linked));
	if (!linked)
	{
		GLint	infoLogLength = 0;
		GLcharARB*	infoLog = NULL;
		GL_ASSERT(glGetObjectParameterivARB(programObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength));
		if (infoLogLength > 0)
		{
			infoLog = (GLcharARB*) malloc(infoLogLength);
			assert(infoLog);
			GL_ASSERT(glGetInfoLogARB(programObject, infoLogLength, &infoLogLength, infoLog));
		}
		fprintf(stderr, "GLSL link error: %s\n", infoLog);
		if (infoLog)
			free(infoLog);
		GL_ASSERT(glDeleteObjectARB(programObject));
		exit(-1);
	}

	return programObject;
}

/*----------------------------------------------------------------------------*/

static GLhandleARB shader_compile(GLenum type, const char* shader)
{
	GLhandleARB	object;
	GLint	compiled;
	GLint	len;

	object = glCreateShader(type);
	assert(object);

	len = (GLint) strlen(shader);
	GL_ASSERT(glShaderSourceARB(object, 1, &shader, &len));
	GL_ASSERT(glCompileShaderARB(object));

	compiled = 0;
	GL_ASSERT(glGetShaderiv(object, GL_COMPILE_STATUS, &compiled));
	if (compiled == GL_FALSE)
	{
		GLint		infoLogLength;
		GLcharARB*	infoLog = NULL;

		GL_ASSERT(glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength));
		if (infoLogLength > 0)
		{
			infoLog = (GLcharARB*) malloc(infoLogLength);
			assert(infoLog);
			GL_ASSERT(glGetInfoLogARB(object, infoLogLength, &infoLogLength, infoLog));
		}
		fprintf(stderr, "GLSL compile error: %s\n", infoLog);
		if (infoLog)
			free(infoLog);
		GL_ASSERT(glDeleteObjectARB(object));
		exit(-1);
	}

	return(object);
}

/** EOF ***********************************************************************/
