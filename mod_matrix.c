/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "stone.h"
#include "modifier.h"
#include "stdmath.h"
#include "blenlib.h"

void _stone_modifier_matrix( s_vertex *v, void *data)
{
	t_mn_mat4 *mat = ( t_mn_mat4 *) data;
	mul_m4_v3( mat->m, v->co);
}

void stone_modifier_matrix( t_stone *stone, s_modifier *mod)
{
	stone_apply_vertex( stone, _stone_modifier_matrix, mod->data);
}

void stone_add_modifier_matrix( t_stone *stone, t_mn_mat4 *mat)
{
	s_modifier *mod = modifier_new("matrix", mat, stone_modifier_matrix);
	stone_add_modifier( stone, mod);
}
