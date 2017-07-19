/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __MUD_SPLINE_H__
#define __MUD_SPLINE_H__

#include "tinyspline.h"

#define SPLINE_DEFAULT 0
#define SPLINE_BEZIERS 1


typedef struct Spline
{
	int degree;
	int dimension;
	int count;
	tsBSpline spline;

}t_spline;

t_spline *mud_spline_new( int degree, int dimension, int count);
void mud_spline_point_set( t_spline *self, int indice, float x, float y, float z);
void mud_spline_eval( t_spline *self, float p, float r[3]);


#endif
