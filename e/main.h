/*
 *	main.h
 *	glsl_mix
 *
 *	Created by Raphael Lemoine on 01/30/12.
 *	Copyright 2012 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>

/*----------------------------------------------------------------------------*/

#if (!defined FALSE)
#define	FALSE	(0)
#endif	/*	(!defined FALSE)	*/
#if (!defined TRUE)
#define	TRUE	(1)
#endif	/*	(!defined TRUE)	*/

/*----------------------------------------------------------------------------*/

#if (0)
#if (defined NDEBUG)

#define	GL_ASSERT(a)	a

#else	/*	(defined NDEBUG)	*/

#define	GL_ASSERT(a)														\
{																			\
	GLenum _error;															\
																			\
	a;																		\
	_error = glGetError();													\
	if (_error != GL_NO_ERROR)												\
	{																		\
		do																	\
		{																	\
			fprintf(stderr, #a ": error 0x%04X\n", _error);					\
			_error = glGetError();											\
		} while (_error != GL_NO_ERROR);									\
		assert(0);															\
	}																		\
}

#endif	/*	(defined NDEBUG)	*/
#endif

/*----------------------------------------------------------------------------*/

union ptr_t
{
	void*		v;
	uint8_t*	u8;
	uint16_t*	u16;
	uint32_t*	u32;
	int8_t*		s8;
	int16_t*	s16;
	int32_t*	s32;
};

typedef union ptr_t		ptr_t;

struct pixmap_t
{
	int			width;
	int			height;
	int			alpha;
	int			rowbytes;
	ptr_t		datas;
};

typedef struct pixmap_t		pixmap_t;

struct rect_t
{
	int		x;
	int		y;
	int		w;
	int		h;
};

typedef struct rect_t		rect_t;

/** EOF ***********************************************************************/

