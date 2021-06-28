/*
 *	MixedWindow.m
 *	glsl_mix
 *
 *	Created by Raphaël Lemoine on 11/13/11.
 *	Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>

#include <png.h>

#include "main.h"
#include "mixedwindow.h"
#include "glslshader.h"

/*----------------------------------------------------------------------------*/

Display*	display = NULL;

/*----------------------------------------------------------------------------*/

extern char	_binary___resources_glsl_mix_vs_start, _binary___resources_glsl_mix_vs_end;
extern char	_binary___resources_glsl_mix_fs_start, _binary___resources_glsl_mix_fs_end;

/*----------------------------------------------------------------------------*/

extern char	_binary_resources_checkerboard_png_start, _binary_resources_checkerboard_png_end;

extern char	_binary_resources_const_cam1_png_start, _binary_resources_const_cam1_png_end;
extern char	_binary_resources_const_cam2_png_start, _binary_resources_const_cam2_png_end;
extern char	_binary_resources_const_cam3_png_start, _binary_resources_const_cam3_png_end;
extern char	_binary_resources_const_cam4_png_start, _binary_resources_const_cam4_png_end;
extern char	_binary_resources_const_cam5_png_start, _binary_resources_const_cam5_png_end;
extern char	_binary_resources_const_cam6_png_start, _binary_resources_const_cam6_png_end;
extern char	_binary_resources_const_cam7_png_start, _binary_resources_const_cam7_png_end;
extern char	_binary_resources_const_cam8_png_start, _binary_resources_const_cam8_png_end;

extern char	_binary_resources_rgb_cam1_png_start, _binary_resources_rgb_cam1_png_end;
extern char	_binary_resources_rgb_cam2_png_start, _binary_resources_rgb_cam2_png_end;
extern char	_binary_resources_rgb_cam3_png_start, _binary_resources_rgb_cam3_png_end;
extern char	_binary_resources_rgb_cam4_png_start, _binary_resources_rgb_cam4_png_end;
extern char	_binary_resources_rgb_cam5_png_start, _binary_resources_rgb_cam5_png_end;
extern char	_binary_resources_rgb_cam6_png_start, _binary_resources_rgb_cam6_png_end;
extern char	_binary_resources_rgb_cam7_png_start, _binary_resources_rgb_cam7_png_end;
extern char	_binary_resources_rgb_cam8_png_start, _binary_resources_rgb_cam8_png_end;

extern char	_binary_resources_rgba_cam1_png_start, _binary_resources_rgba_cam1_png_end;
extern char	_binary_resources_rgba_cam2_png_start, _binary_resources_rgba_cam2_png_end;
extern char	_binary_resources_rgba_cam3_png_start, _binary_resources_rgba_cam3_png_end;
extern char	_binary_resources_rgba_cam4_png_start, _binary_resources_rgba_cam4_png_end;
extern char	_binary_resources_rgba_cam5_png_start, _binary_resources_rgba_cam5_png_end;
extern char	_binary_resources_rgba_cam6_png_start, _binary_resources_rgba_cam6_png_end;
extern char	_binary_resources_rgba_cam7_png_start, _binary_resources_rgba_cam7_png_end;
extern char	_binary_resources_rgba_cam8_png_start, _binary_resources_rgba_cam8_png_end;

static char*	const_pngs[8 * 2] =
{
	&_binary_resources_const_cam1_png_start, &_binary_resources_const_cam1_png_end,
	&_binary_resources_const_cam2_png_start, &_binary_resources_const_cam2_png_end,
	&_binary_resources_const_cam3_png_start, &_binary_resources_const_cam3_png_end,
	&_binary_resources_const_cam4_png_start, &_binary_resources_const_cam4_png_end,
	&_binary_resources_const_cam5_png_start, &_binary_resources_const_cam5_png_end,
	&_binary_resources_const_cam6_png_start, &_binary_resources_const_cam6_png_end,
	&_binary_resources_const_cam7_png_start, &_binary_resources_const_cam7_png_end,
	&_binary_resources_const_cam8_png_start, &_binary_resources_const_cam8_png_end
};

static char*	rgb_pngs[8 * 2] =
{
	&_binary_resources_rgb_cam1_png_start, &_binary_resources_rgb_cam1_png_end,
	&_binary_resources_rgb_cam2_png_start, &_binary_resources_rgb_cam2_png_end,
	&_binary_resources_rgb_cam3_png_start, &_binary_resources_rgb_cam3_png_end,
	&_binary_resources_rgb_cam4_png_start, &_binary_resources_rgb_cam4_png_end,
	&_binary_resources_rgb_cam5_png_start, &_binary_resources_rgb_cam5_png_end,
	&_binary_resources_rgb_cam6_png_start, &_binary_resources_rgb_cam6_png_end,
	&_binary_resources_rgb_cam7_png_start, &_binary_resources_rgb_cam7_png_end,
	&_binary_resources_rgb_cam8_png_start, &_binary_resources_rgb_cam8_png_end
};

static char*	rgba_pngs[8 * 2] =
{
	&_binary_resources_rgba_cam1_png_start, &_binary_resources_rgba_cam1_png_end,
	&_binary_resources_rgba_cam2_png_start, &_binary_resources_rgba_cam2_png_end,
	&_binary_resources_rgba_cam3_png_start, &_binary_resources_rgba_cam3_png_end,
	&_binary_resources_rgba_cam4_png_start, &_binary_resources_rgba_cam4_png_end,
	&_binary_resources_rgba_cam5_png_start, &_binary_resources_rgba_cam5_png_end,
	&_binary_resources_rgba_cam6_png_start, &_binary_resources_rgba_cam6_png_end,
	&_binary_resources_rgba_cam7_png_start, &_binary_resources_rgba_cam7_png_end,
	&_binary_resources_rgba_cam8_png_start, &_binary_resources_rgba_cam8_png_end
};

struct pngdata_t
{
	const char*	data;
	size_t		len;
	const char*	curdata;
	size_t		curlen;
};

typedef struct pngdata_t		pngdata_t;

/*----------------------------------------------------------------------------*/

Window				rootWindow;
static GLuint		srcTexturesIDs[8];
static GLuint		dstTextureID;
static GLuint		bgndTextureID;
static int			hasAlpha;
static int			lobeTracking = TRUE;
static int			textureType = -1;
static GLhandleARB	mixShader = 0;

static pixmap_t*	pixmap_create(const char* pngstart, const char* pngend);
static void			pixmap_delete(pixmap_t* pixmap);
static void*		PNG_malloc(png_struct* png, size_t size);
static void			PNG_free(png_struct* png, void* ptr);
static void			PNG_read(png_structp png, png_bytep data, png_size_t size);

/******************************************************************************/

Window window_create(rect_t* rect)
{
	Window					window;
	static unsigned long	black, white;
	XVisualInfo*			visual;
	GLint					attributes[] =
	{
		GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None
	};
	GLXContext				context;
	pixmap_t* 				pixmap;
	size_t					len;
	char*					vertex;
	char*					fragment;

	if (display == NULL)
	{
		int				screen;
	
		display = XOpenDisplay(NULL);
		assert(display);
		screen = DefaultScreen(display);
		rootWindow = XRootWindow(display, screen);
		black = BlackPixel(display, screen);
		white = WhitePixel(display, screen);
	}


	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
		rect->x, rect->y, rect->w, rect->h,
		2, black, white);


	XStoreName(display, window, "glsl_mix");

	visual = glXChooseVisual(display, 0, attributes);
	assert(visual);

	context = glXCreateContext(display, visual, NULL, GL_TRUE);
	assert(context);
	glXMakeCurrent(display, window, context);

	GL_ASSERT(glEnable(GL_TEXTURE_RECTANGLE_ARB));

	GL_ASSERT(glGenTextures(8, srcTexturesIDs));
	GL_ASSERT(glGenTextures(1, &dstTextureID));
	GL_ASSERT(glGenTextures(1, &bgndTextureID));

	GL_ASSERT(glMatrixMode(GL_MODELVIEW));
	GL_ASSERT(glLoadIdentity());
	GL_ASSERT(glMatrixMode(GL_PROJECTION));
	GL_ASSERT(glLoadIdentity());
	GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

	/*	Create background (checkerboard) pixmap
	 */
	{
		pixmap = pixmap_create(&_binary_resources_checkerboard_png_start, &_binary_resources_checkerboard_png_end);
		assert(pixmap);

		GL_ASSERT(glBindTexture(GL_TEXTURE_2D, bgndTextureID));
		GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
			(GLsizei) pixmap->width, (GLsizei) pixmap->height, 0, pixmap->alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
					pixmap->datas.v));
		pixmap_delete(pixmap);
	}

	len = (&_binary___resources_glsl_mix_vs_end - &_binary___resources_glsl_mix_vs_start) + 1;
	vertex = malloc(len);
	assert(vertex);
	memset(vertex, 0, len);
	memcpy(vertex, &_binary___resources_glsl_mix_vs_start, len - 1);

	len = (&_binary___resources_glsl_mix_fs_end - &_binary___resources_glsl_mix_fs_start) + 1;
	fragment = malloc(len);
	assert(fragment);
	memset(fragment, 0, len);
	memcpy(fragment, &_binary___resources_glsl_mix_fs_start, len - 1);

	mixShader = shader_create(vertex, fragment);
	assert(mixShader);

	XSelectInput(display, window, KeyPressMask | ExposureMask | StructureNotifyMask);

	window_setimage(window, 2);

	XMapWindow(display, window);

	return(window);
}

/*----------------------------------------------------------------------------*/

void window_destroy(Window window)
{
	assert(window);

	XDestroyWindow(display, window);

	XCloseDisplay(display);
}

/*----------------------------------------------------------------------------*/

void window_setimage(Window window, int type)
{
	int		i;

	assert(window);
	assert(type >= 0);
	assert(type <= 2);
	if (type != textureType)
	{
		char**	pngs;

		switch (type)		
		{
			case 0:
			pngs = const_pngs;
			break;

			case 1:
			pngs = rgb_pngs;
			break;

			case 2:
			pngs = rgba_pngs;
			break;

			default:
			assert(0);
			pngs = NULL;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			pixmap_t* pixmap;

			pixmap = pixmap_create(pngs[(i * 2) + 0], pngs[(i * 2) + 1]);
			assert(pixmap);
			hasAlpha = pixmap->alpha;

			GL_ASSERT(glBindTexture(GL_TEXTURE_RECTANGLE_ARB, srcTexturesIDs[i]));
			GL_ASSERT(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_ASSERT(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_ASSERT(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_ASSERT(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL_ASSERT(glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, pixmap->alpha ? GL_RGBA8 : GL_RGB8,
				(GLsizei) pixmap->width, (GLsizei) pixmap->height, 0, pixmap->alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
				pixmap->datas.v));

			pixmap_delete(pixmap);
		}
		
		textureType = type;
	}
}

/*----------------------------------------------------------------------------*/

int window_gettracking(void)
{
	return(lobeTracking);
}

/*----------------------------------------------------------------------------*/

void window_toggletracking(void)
{
	lobeTracking = !lobeTracking;
}

/*----------------------------------------------------------------------------*/

void window_redraw(Window window)
{
	XWindowAttributes	attributes;
	Window				w;
	int					i, location, lobe;
	rect_t				rect;

	
	{
		XTranslateCoordinates(display, window, rootWindow, 0, 0, &rect.x, &rect.y, &w);
		XGetWindowAttributes(display, window, &attributes);
		rect.w = attributes.width;
		rect.h = attributes.height;
	}

	GL_ASSERT(glViewport(0, 0, rect.w, rect.h));

	GL_ASSERT(glUseProgramObjectARB(mixShader));

	/*	Set source textures (viewpoints).
	 */
	for (i = 0; i < 8; i++)
	{
		char	texName[32];

		sprintf(texName, "srcTextures[%d]", i);
		GL_ASSERT(location = glGetUniformLocationARB(mixShader, texName));
		assert(location != -1);
		GL_ASSERT(glActiveTexture(GL_TEXTURE0 + i));
		GL_ASSERT(glBindTexture(GL_TEXTURE_RECTANGLE_ARB, srcTexturesIDs[i]));
		GL_ASSERT(glUniform1iARB(location, i));
	}

	/*	Set lobe offset.
	 */
	GL_ASSERT(location = glGetUniformLocationARB(mixShader, "lobeOffset"));
	assert(location != -1);
	lobe = 0;
	if (lobeTracking)
	{
		lobe = ((rect.x * 3) - rect.y);
	}
	GL_ASSERT(glUniform1fARB(location, (GLfloat) lobe));

	/*	Set alpha-channel flag.
	 */
	GL_ASSERT(location = glGetUniformLocationARB(mixShader, "hasAlpha"));
	assert(location != -1);
	GL_ASSERT(glUniform1iARB(location, (GLint) hasAlpha));

	/*	Set background texture.
	 */
	GL_ASSERT(location = glGetUniformLocationARB(mixShader, "bgndTexture"));
	assert(location != -1);
	GL_ASSERT(glActiveTexture(GL_TEXTURE8));
	GL_ASSERT(glBindTexture(GL_TEXTURE_2D, bgndTextureID));
	GL_ASSERT(glUniform1iARB(location, 8));


	/*	Set mixed height.
	 */
	GL_ASSERT(location = glGetUniformLocationARB(mixShader, "dstHeight"));
	assert(location != -1);
	GL_ASSERT(glUniform1fARB(location, (GLfloat) rect.h));

/*	GL_ASSERT(glClearColor(0.5, 1.0, 0.0, 1.0));
	GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));*/

	glBegin(GL_QUADS);

	/*	Top-right vertex.
	 */
	glTexCoord2f((GLfloat) 960.0f, (GLfloat) 0.0f);
	glVertex2f((GLfloat) 1.0f, (GLfloat) 1.0f);

	/*	Bottom-right vertex.
	 */
	glTexCoord2f((GLfloat) 960.0f, (GLfloat) 540.0f);
	glVertex2f((GLfloat) 1.0f, (GLfloat) -1.0f);

	/*	Bottom-left vertex.
	 */
	glTexCoord2f((GLfloat) 0.0f, (GLfloat) 540.0f);
	glVertex2f((GLfloat) -1.0f, (GLfloat) -1.0f);

	/*	Top-left vertex.
	 */
	glTexCoord2f((GLfloat) 0.0f, (GLfloat) 0.0f);
	glVertex2f((GLfloat) -1.0f, (GLfloat) 1.0f);

	GL_ASSERT(glEnd());

//	GL_ASSERT(glFlush());

	GL_ASSERT(glUseProgramObjectARB(0));

	glXSwapBuffers(display, window);

}

/******************************************************************************/

static pixmap_t* pixmap_create(const char* pngstart, const char* pngend)
{
	pngdata_t	pngdata;
	png_structp	png_ptr;
	png_infop	info_ptr;
	png_uint_32	y, w, h;
	int			bpc, type, alpha;
	size_t		rowbytes;
	uint8_t**	rowptrs;
	ptr_t		temp;
	pixmap_t*	pixmap;

	png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL, NULL, PNG_malloc, PNG_free);
	assert(png_ptr);
	info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);
	if (setjmp(png_jmpbuf(png_ptr)))
 //   if (setjmp(png_ptr->jmpbuf) != 0)
	{
		fprintf(stderr, "png file corrupted, exiting.");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		exit(1);
    }

	pngdata.data = pngstart;
	pngdata.len = pngend - pngstart;
	pngdata.curdata = pngdata.data;
	pngdata.curlen = pngdata.len;
	png_set_read_fn(png_ptr, (png_voidp) &pngdata, PNG_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bpc, &type, NULL, NULL, NULL);

	switch (type)
	{
		case PNG_COLOR_TYPE_GRAY:				/*	Grayscale, 1/2/4/8/16 bits per channel	*/
		if (bpc < 8)
			png_set_gray_1_2_4_to_8(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		alpha = FALSE;
		break;

		case PNG_COLOR_TYPE_RGB:				/*	RGB, 8/16 bits per channel	*/
		alpha = FALSE;
		break;

		case PNG_COLOR_TYPE_PALETTE:			/*	indexed-colour, 1/2/4/8 bits per pixel	*/
		png_set_palette_to_rgb(png_ptr);
		alpha = FALSE;
		break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:			/*	Grayscale + Alpha, 8/16 bits per channel	*/
		png_set_gray_to_rgb(png_ptr);
		alpha = TRUE;
		break;

		case PNG_COLOR_TYPE_RGB_ALPHA:			/*	RGB + Alpha, 8/16 bits per channel	*/
		alpha = TRUE;
		break;

		default:
		assert(0);
		break;
	}

	if (bpc == 16)
		png_set_strip_16(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	rowbytes = (uint32_t) png_get_rowbytes(png_ptr, info_ptr);

	temp.v = malloc(rowbytes * h);
	assert(temp.v);

	rowptrs = malloc(h * sizeof(uint8_t*));
	assert(rowptrs);
	for (y = 0; y < h; y++)
		rowptrs[y] = temp.u8 + (y * rowbytes);

    png_read_image(png_ptr, rowptrs);
	
    png_read_end(png_ptr, NULL);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


	pixmap = malloc(sizeof(pixmap_t));
	assert(pixmap);

	pixmap->width = w;
	pixmap->height = h;
	pixmap->alpha = alpha;
	pixmap->rowbytes = rowbytes;
	pixmap->datas = temp;
	return(pixmap);
}

/*----------------------------------------------------------------------------*/

static void pixmap_delete(pixmap_t* pixmap)
{
	assert(pixmap);

	if (pixmap->datas.v)
		free(pixmap->datas.v);
	free(pixmap);
}

/*----------------------------------------------------------------------------*/

static void* PNG_malloc(png_struct __attribute__((__unused__))* png, size_t size)
{
	return(malloc(size));
}

/*----------------------------------------------------------------------------*/

static void PNG_free(png_struct __attribute__((__unused__))* png, void* ptr)
{
	free(ptr);
}

/*----------------------------------------------------------------------------*/

static void PNG_read(png_structp png, png_bytep data, png_size_t size)
{
	pngdata_t*	pngdata;

	pngdata = png->io_ptr;
	if (pngdata->curlen >= size)
	{
		memcpy(data, pngdata->curdata, size);
		pngdata->curdata += size;
		pngdata->curlen -= size;
	}
	else
	{
		assert(0);
	}
}

/** EOF ***********************************************************************/

