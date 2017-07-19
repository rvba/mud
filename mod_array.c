/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
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

typedef struct Array_Data
{
	int count;
	t_mn_mat4 *matrix;

}t_array_data;

static t_array_data *array_data_new( int count, t_mn_mat4 *mat)
{
	t_array_data *data = ( t_array_data *) calloc( 1, sizeof( t_array_data));
	data->count = count;
	data->matrix = mat;
	return data;
}

float unity[4][4] = {      
	{ 1.0, 0.0, 0.0, 0.0}, 
	{ 0.0, 1.0, 0.0, 0.0}, 
	{ 0.0, 0.0, 1.0, 0.0}, 
	{ 0.0, 0.0, 0.0, 1.0}  
};

void mud_modifier_array( t_mud *mud, s_modifier *mod)
{
	t_array_data *data = ( t_array_data *) mod->data;

	int i;
	int count = data->count;

	t_mn_mat4 *mat = data->matrix;
	t_mn_mat4 *matrix = mn_mat4_new( unity);

	t_mud *copy = mud_copy( mud);

	for( i = 0; i < count; i++)
	{
		t_mud *c = mud_copy( copy);

		unit_m4( matrix->m);
		int j;
		for( j = 0; j < i; j++)
		{
			mul_m4_m4m4( matrix->m, matrix->m, mat->m);
		}

		mud_add_modifier_matrix( c, matrix);
		mud_apply_modifiers( c);
		mud_merge( c, mud);
		// memory leak
		// "c" mud copy cannot be freed 
		// his points are used in "final" merge
	}

	mud_free( copy);
}

void mud_add_modifier_array( struct Stone *mud, int count, t_mn_mat4 *mat)
{
	t_array_data *data = array_data_new( count, mat);
	s_modifier *mod = modifier_new("array", data, mud_modifier_array);
	mud_add_modifier( mud, mod);
}

