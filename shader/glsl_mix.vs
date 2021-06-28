/*
 *	glsl_mix.vs
 *	glsl_mix
 *
 *	Created by Raphael Lemoine on 11/9/11.
 *	Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

void main(void)
{
	/*	Do not modify the vertex.
	 */
	gl_Position = gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

/** EOF ***********************************************************************/
