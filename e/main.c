/*
 *	main.c
 *	glsl_mix
 *
 *	Created by Raphael Lemoine on 01/30/12.
 *	Copyright 2012 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

#include "main.h"
#include "mixedwindow.h"
#include "glslshader.h"

/*----------------------------------------------------------------------------*/

int main(int argc, char* argv[]);

/******************************************************************************/

int main(int __attribute__((__unused__)) argc, char __attribute__((__unused__))* argv[])
{
	Window	window;
	rect_t	rect;
	int		running;
	Atom	wmDeleteMessage;

	rect.x = 100;
	rect.y = 100;
	rect.w = 960;
	rect.h = 540;
	window = window_create(&rect);
	assert(window);

	wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wmDeleteMessage, 1);

	running = TRUE;
	do
	{
		XEvent	event;

		XNextEvent(display, &event);
		switch (event.type)
		{
			case KeyPress:
			{
				char	string[32];
				int		count;
				KeySym	keysym;

				count = XLookupString(&event.xkey, string, 32, &keysym, NULL);
				if (count > 0)
				{
					if (strcmp(string, "1") == 0)
						window_setimage(event.xkey.window, 0);
					else if (strcmp(string, "2") == 0)
						window_setimage(event.xkey.window, 1);
					else if (strcmp(string, "3") == 0)
						window_setimage(event.xkey.window, 2);
					else if (strcmp(string, " ") == 0)
						window_toggletracking();

					window_redraw(window);
				}
			}
			break;

			case Expose:
			{
				if (event.xexpose.count == 0)
				{
					window_redraw(window);
				}
			}
			break;

			case ConfigureNotify:
			{
				if (((rect.w != event.xconfigure.width) || (rect.h != event.xconfigure.height))
					|| window_gettracking())
				{
					window_redraw(window);
				}
			}
			break;

			case ClientMessage:
			{
				if (event.xclient.data.l[0] == (long) wmDeleteMessage)
					running = FALSE;
			}
			break;

			default:
			{
				const char*	name;

				switch (event.type)
				{
/*					case KeyPress:			name = "KeyPress";			break;*/
					case KeyRelease:		name = "KeyRelease";		break;
					case ButtonPress:		name = "ButtonPress";		break;
					case ButtonRelease:		name = "ButtonRelease";		break;
					case MotionNotify:		name = "MotionNotify";		break;
					case EnterNotify:		name = "EnterNotify";		break;
					case LeaveNotify:		name = "LeaveNotify";		break;
					case FocusIn:			name = "FocusIn";			break;
					case FocusOut:			name = "FocusOut";			break;
					case KeymapNotify:		name = "KeymapNotify";		break;
					case GraphicsExpose:	name = "GraphicsExpose";	break;
/*					case Expose:			name = "Expose";			break;*/
					case NoExpose:			name = "NoExpose";			break;
					case CirculateRequest:	name = "CirculateRequest";	break;
					case ConfigureRequest:	name = "ConfigureRequest";	break;
					case MapRequest:		name = "MapRequest";		break;
					case ResizeRequest:		name = "ResizeRequest";		break;
					case CirculateNotify:	name = "CirculateNotify";	break;
/*					case ConfigureNotify:	name = "ConfigureNotify";	break;*/
					case CreateNotify:		name = "CreateNotify";		break;
					case DestroyNotify:		name = "DestroyNotify";		break;
					case GravityNotify:		name = "GravityNotify";		break;
					case MapNotify:			name = "MapNotify";			break;
					case MappingNotify:		name = "MappingNotify";		break;
					case ReparentNotify:	name = "ReparentNotify";	break;
					case UnmapNotify:		name = "UnmapNotify";		break;
					case VisibilityNotify:	name = "VisibilityNotify";	break;
					case ColormapNotify:	name = "ColormapNotify";	break;
/*					case ClientMessage:		name = "ClientMessage";		break;*/
					case PropertyNotify:	name = "PropertyNotify";	break;
					case SelectionClear:	name = "SelectionClear";	break;
					case SelectionNotify:	name = "SelectionNotify";	break;
					case SelectionRequest:	name = "SelectionRequest";	break;
					default:				name = "<Unknown>";			break;
				}
/*				fprintf(stderr, "message: %s\n", name);
				fflush(stderr);*/
			}
			break;
		}
	} while (running);

	window_destroy(window);

	return(0);
}

/** EOF ***********************************************************************/

