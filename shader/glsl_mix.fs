/*
 *	glsl_mix.fs
 *	glsl_mix
 *
 *	Created by Raphael Lemoine on 11/9/11.
 *	Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/

/*	The 8 source viewpoints (GL_TEXTURE_RECTANGLE_2D).
 */
uniform sampler2DRect	srcTextures[8];

/*	The quad's display position, for lobe tracking ((dispx * 3) + dispy).
 */
uniform	float			lobeOffset;

/*	A simple boolean for alpha channel handling.
 */
uniform	bool			hasAlpha;

/*	The background texture (GL_TEXTURE_2D), used when hasAlpha is TRUE.
 */
uniform	sampler2D		bgndTexture;

/*	The display's height, used by the background sampling.
 */
uniform	float			dstHeight;

/*----------------------------------------------------------------------------*/

void main(void)
{
	vec4		colors[8 + 2];
	int			vp;
	vec3		color;

	/*	1: Sample all viewpoints into the 'colors[]' array.
	 */
	{
		colors[0] = texture2DRect(srcTextures[0], gl_TexCoord[0].st);
		colors[1] = texture2DRect(srcTextures[1], gl_TexCoord[0].st);
		colors[2] = texture2DRect(srcTextures[2], gl_TexCoord[0].st);
		colors[3] = texture2DRect(srcTextures[3], gl_TexCoord[0].st);
		colors[4] = texture2DRect(srcTextures[4], gl_TexCoord[0].st);
		colors[5] = texture2DRect(srcTextures[5], gl_TexCoord[0].st);
		colors[6] = texture2DRect(srcTextures[6], gl_TexCoord[0].st);
		colors[7] = texture2DRect(srcTextures[7], gl_TexCoord[0].st);

		/*	While there are only 8 viewpoints, colors[] array size is 10: we
		 *	copy colors[0] (colors[1]) into colors[8] (colors[9]) because
		 *	we use (vp + 0), (vp + 1), and (vp + 2) offsets in colors[].
		 *	This way, we don't have to calculate '(vp + n) % 8' at every
		 *	subpixel.
		 */
		colors[8] = colors[0];
		colors[9] = colors[1];
	}

	/*	2: Calculate which viewpoints are needed by the fragment, taking into
	 *	account its physical position on the screen (lobeOffset).
	 */
	{
		float		xy;

		xy = (gl_FragCoord.x * 3.0) + gl_FragCoord.y + lobeOffset + 0.5;
		vp = 7 - int(mod(xy, 8.0));
	}

	/*	3: Get the R/G/B values from the right viewpoints.
	 */
	color = vec3(colors[vp + 2].r, colors[vp + 1].g, colors[vp + 0].b);

	/*	4: Blend the subpixels with the background pattern, if needed. You
	 *	might probably want to change this code in order to blend the mixed
	 *	image with your own background pattern/image.
	 */
	if (hasAlpha)
	{
		vec2	coord;
		vec3	bgnd;
		vec3	alphas;

		/*	Get the background color for the fragment. 32 is the
		 *	checkerboard image width/height (see checkerboard.png), which
		 *	is tiled (GL_REPEAT). Note that the background texture is of
		 *	type GL_TEXTURE_2D, and so use a texture2D sampler (the 8
		 *	viewpoints are of type GL_TEXTURE_RECTANGLE_ARB, and use
		 *	texture2DRect samplers).
		 */
		coord.x = gl_FragCoord.x * (1.0 / 32.0);

		/*	We want the checkerboard vertical origin to be the top of the
		 *	image.
		 */
		coord.y = (dstHeight - gl_FragCoord.y) * (1.0 / 32.0);

		/*	Sample the checkerboard pattern (point sampling).
		 */
		bgnd = texture2D(bgndTexture, coord).rgb;

		/*	Get the alpha values of the viewpoints.
		 */
		alphas = vec3(colors[vp + 2].a, colors[vp + 1].a, colors[vp + 0].a);

		/*	Blend the viewpoints color with the background color, using
		 *	the proper alpha for each subpixel.
		 */
		color = mix(bgnd, color, alphas);
	}

	/*	4: Store the R/G/B/X values into the final color.
	 */
	gl_FragColor = vec4(color, 1.0);
}

/** EOF ***********************************************************************/
