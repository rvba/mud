/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "blenlib.h"
#include "modifier.h"
#include "llist.h"
#include "mud.h"
#include "stdmath.h"
#include "stdvec.h"

typedef struct Rotation_Data
{
	float v[3];
	float a;

}t_rotation_data;

static t_rotation_data *rotation_data_new( const float v[3], const float a)
{
	t_rotation_data *data = ( t_rotation_data *) calloc( 1, sizeof( t_rotation_data));
	data->v[0] = v[0];
	data->v[1] = v[1];
	data->v[2] = v[2];
	data->a = a;
	return data;
}

void mud_modifier_rotation( t_mud *mud, s_modifier *mod)
{
	t_rotation_data *data = ( t_rotation_data *) mod->data;

	t_lnode *node;
	float angle = deg_to_rad(data->a);
	float *v = data->v;
	printf("%f %f %f\n", v[0], v[1], v[2]);
	for( node = mud->vertex->first; node; node = node->next)
	{
		s_vertex *v = ( s_vertex *) node->data;
		rotate_v3_v3v3fl( v->co, v->co, data->v, angle);
	}
}

void mud_add_modifier_rotation( struct Mud *mud, const float v[3], const float a)
{
	t_rotation_data *data = rotation_data_new( v, a);
	s_modifier *mod = modifier_new("rotation", data, mud_modifier_rotation);
	mud_add_modifier( mud, mod);
}

