/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "stone_spline.h"


t_spline *stone_spline_new( int degree, int dimension, int count)
{
	t_spline *spline = ( t_spline *) malloc( sizeof( t_spline));
	spline->degree = degree;
	spline->dimension = dimension;
	spline->count = count;

	ts_bspline_new(
		degree,			// (3) degree of spline
		dimension,		// (3) dimension of each point
		count,			// number of control points
		TS_CLAMPED	,	// used to hit first and last control point
		&spline->spline		// the spline to setup
	);

	return spline;
}

void stone_spline_point_set( t_spline *self, int indice, float x, float y, float z)
{
	if( indice < self->count)
	{
		int d = self->dimension;
		self->spline.ctrlp[(indice*d)+0] = x;
		self->spline.ctrlp[(indice*d)+1] = y;
		self->spline.ctrlp[(indice*d)+2] = z;
	}

	#if 0
	printf("stone spline set: %d %f %f %f\n",indice,  x, y, z);
	#endif
}

void stone_spline_change_type( t_spline *self, int type)
{
	if( type == SPLINE_BEZIERS)
	{
		/* should have a local copy here to compute the stuff... */
		ts_bspline_to_beziers(&self->spline, &self->spline);
	}
}

void stone_spline_eval( t_spline *self, float p, float r[3])
{
	tsDeBoorNet net;
	ts_bspline_evaluate(&self->spline, p, &net);

	#if 0
	printf("stone spline eval: (%f) %f %f %f\n", p, net.result[0], net.result[1], net.result[2]);
	#endif

	r[0] = net.result[0];
	r[1] = net.result[1];
	r[2] = net.result[2];
}






