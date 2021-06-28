/*
 *	mixedwindow.h
 *	glsl_mix
 *
 *	Created by Raphaël Lemoine on 11/13/11.
 *	Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#include <assert.h>
#include <X11/Xlib.h>

/******************************************************************************/

extern	Display*	display;

/*----------------------------------------------------------------------------*/

Window	window_create(rect_t* rect);
void	window_destroy(Window window);
void	window_setimage(Window window, int type);
int		window_gettracking(void);
void	window_toggletracking(void);
void	window_redraw(Window window);

/** EOF ***********************************************************************/

