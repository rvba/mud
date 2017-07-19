/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "mud.h"
#include "modifier.h"
#include "stdmath.h"
#include "blenlib.h"

void _mud_modifier_matrix( s_vertex *v, void *data)
{
	t_mn_mat4 *mat = ( t_mn_mat4 *) data;
	mul_m4_v3( mat->m, v->co);
}

void mud_modifier_matrix( t_mud *mud, s_modifier *mod)
{
	mud_apply_vertex( mud, _mud_modifier_matrix, mod->data);
}

void mud_add_modifier_matrix( t_mud *mud, t_mn_mat4 *mat)
{
	s_modifier *mod = modifier_new("matrix", mat, mud_modifier_matrix);
	mud_add_modifier( mud, mod);
}
