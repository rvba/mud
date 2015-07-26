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
#include "stone.h"
#include "stdmath.h"

/*
 * Largely inspired by Blender skin modifier
 *
 */

typedef struct Skin
{
	s_edge *edge;
	t_mn_mat3 *mat;
	float co[4][3];
	s_vertex *a;
	s_vertex *b;
	s_vertex *c;
	s_vertex *d;

}t_skin;

static t_llist *skin_nodes = NULL;

void skin_copy_mat( float mat[3][3], float m[3][3])
{
	mat[0][0] = m[0][0];
	mat[0][1] = m[0][1];
	mat[0][2] = m[0][2];
	mat[1][0] = m[1][0];
	mat[1][1] = m[1][1];
	mat[1][2] = m[1][2];
	mat[2][0] = m[2][0];
	mat[2][1] = m[2][1];
	mat[2][2] = m[2][2];
}

static void create_frame(t_skin *frame)
{
	float mat[3][3];
	skin_copy_mat(mat, frame->mat->m);

	float radius[2] = {.1,.1};
	float offset = 0;
	float co[3];
	co[0] = frame->edge->a->co[0];
	co[1] = frame->edge->a->co[1];
	co[2] = frame->edge->a->co[2];

	float rx[3], ry[3], rz[3];
	int i;

	mul_v3_v3fl(ry, mat[1], radius[0]);
	mul_v3_v3fl(rz, mat[2], radius[1]);
	
	add_v3_v3v3(frame->co[3], co, ry);
	add_v3_v3v3(frame->co[3], frame->co[3], rz);

	sub_v3_v3v3(frame->co[2], co, ry);
	add_v3_v3v3(frame->co[2], frame->co[2], rz);

	sub_v3_v3v3(frame->co[1], co, ry);
	sub_v3_v3v3(frame->co[1], frame->co[1], rz);

	add_v3_v3v3(frame->co[0], co, ry);
	sub_v3_v3v3(frame->co[0], frame->co[0], rz);

	mul_v3_v3fl(rx, mat[0], offset);
	for (i = 0; i < 4; i++)
		add_v3_v3v3(frame->co[i], frame->co[i], rx);
}

void create_points( t_stone *stone, t_skin *skin)
{
	skin->a = stone_add_vertex( stone, skin->co[0][0], skin->co[0][1], skin->co[0][2]);
	skin->b = stone_add_vertex( stone, skin->co[1][0], skin->co[1][1], skin->co[1][2]);
	skin->c = stone_add_vertex( stone, skin->co[2][0], skin->co[2][1], skin->co[2][2]);
	skin->d = stone_add_vertex( stone, skin->co[3][0], skin->co[3][1], skin->co[3][2]);

}
	
static void calc_edge_mat(float mat[3][3], const float a[3], const float b[3])
{
	const float z_up[3] = {0, 0, 1};
	float dot;

	/* X = edge direction */
	sub_v3_v3v3(mat[0], b, a);
	normalize_v3(mat[0]);

	dot = dot_v3v3(mat[0], z_up);
	if (dot > -1 + FLT_EPSILON && dot < 1 - FLT_EPSILON) {
		/* Y = Z cross x */
		cross_v3_v3v3(mat[1], z_up, mat[0]);
		normalize_v3(mat[1]);

		/* Z = x cross y */
		cross_v3_v3v3(mat[2], mat[0], mat[1]);
		normalize_v3(mat[2]);
	}
	else {
		mat[1][0] = 1;
		mat[1][1] = 0;
		mat[1][2] = 0;
		mat[2][0] = 0;
		mat[2][1] = 1;
		mat[2][2] = 0;
	}
}

t_skin *skin_new( void)
{
	t_skin *skin =  ( t_skin *) calloc(1, sizeof( t_skin));
	float m[3][3];
	skin->mat = mn_mat3_new(m);
	return skin;
}

void stone_modifier_skin( t_stone *stone, s_modifier *mod)
{
	skin_nodes = llist_new();

	t_lnode *node;
	t_skin *s = NULL;
	for( node = stone->edge->first; node; node = node->next)
	{
		t_skin *skin = skin_new();
		s_edge *edge =  ( s_edge *) node->data;
		skin->edge = edge;
		calc_edge_mat( skin->mat->m, skin->edge->a->co, skin->edge->b->co);
		create_frame( skin);
		create_points( stone, skin);
		if( s)
		{
			stone_add_face( stone, s->a, s->b, skin->b, skin->a);
			stone_add_face( stone, s->b, s->c, skin->c, skin->b);
			stone_add_face( stone, s->c, s->d, skin->d, skin->c);
			stone_add_face( stone, s->d, s->a, skin->a, skin->d);
		}
		s = skin;
	}
}

void stone_add_modifier_skin( struct Stone *stone)
{
	s_modifier *mod = modifier_new("skin", NULL, stone_modifier_skin);
	stone_add_modifier( stone, mod);
}

