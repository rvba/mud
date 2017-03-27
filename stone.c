/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

/**
 * @file Main Stone file
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stone.h"
#include "modifier.h"
#include "stash.h"
#include "llist.h"

#include "blenlib.h"
#include "stdmath.h"

// New

/**
 * @ref stone_vertex_set
 * @fn void stone_vertex_set( s_vertex *vert, float *v)
 * set x y z
 */
void stone_vertex_set( s_vertex *vert, float *v)
{
	vert->co[0] = v[0];
	vert->co[1] = v[1];
	vert->co[2] = v[2];
}

void stone_face_set( s_face *face, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	face->a = a;
	face->b = b;
	face->c = c;
	face->d = d;
}

s_vertex *stone_vertex_new( int indice, float *v)
{
	s_vertex *vert = ( s_vertex *) calloc( 1, sizeof( s_vertex));
	vert->indice = indice;
	stone_vertex_set( vert, v);
	return vert;
}

s_edge *stone_edge_new( s_vertex *a, s_vertex *b)
{
	s_edge *edge = ( s_edge *) calloc( 1, sizeof( s_edge));
	edge->a = a;
	edge->b = b;
	return edge;
}

s_face *stone_face_new( s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	s_face *face = ( s_face *) calloc( 1, sizeof( s_face));
	stone_face_set( face, a, b, c, d);
	return face;
}

t_stone *stone_new( const char *name)
{
	t_stone *stone = ( t_stone *) calloc( 1, sizeof( t_stone));
	strncpy( stone->name, name, STONE_NAME);
	stone->node = lnode_new( stone);
	stone->vertex = llist_new();
	stone->face = llist_new();
	stone->edge = llist_new();
	stone->modifier = llist_new();
	return stone;
}

/* Get */

s_vertex *stone_get_vertex_by_indice( t_stone *stone, int indice)
{
	t_lnode *node = llist_get_lnode_by_indice( stone->vertex, indice);
	assert( node);
	return ( s_vertex *) node->data;
}

s_face *stone_get_face( t_stone *stone, int indice)
{
	t_lnode *node = llist_get_lnode_by_indice( stone->face, indice);
	assert( node);
	return ( s_face *) node->data;
}

s_vertex *stone_get_vertex( t_stone *stone, int indice)
{
	return stone_get_vertex_by_indice( stone, indice);
}

/* Set */

void stone_vertex_update( t_stone *stone, int indice, float *vector)
{
	s_vertex *vertex = stone_get_vertex( stone, indice);
	if( vertex) stone_vertex_set( vertex, vector);
	else printf("[stone] Error, can't get vertex %d\n", indice);
}

// Free

void stone_vertex_free( s_vertex *v)
{
	free(v);
}

void stone_edge_free( s_edge *e)
{
	free(e);
}

void stone_face_free( s_face *f)
{
	free(f);
}

void stone_free( t_stone *stone)
{
	t_lnode *node;

	for( node = stone->vertex->first; node; node = node->next)
	{
		s_vertex *v = ( s_vertex *) node->data;
		stone_vertex_free( v);
	}

	for( node = stone->edge->first; node; node = node->next)
	{
		s_edge *e = ( s_edge *) node->data;
		stone_edge_free( e);
	}

	for( node = stone->face->first; node; node = node->next)
	{
		s_face *f = ( s_face *) node->data;
		stone_face_free( f);
	}

	for( node = stone->modifier->first; node; node = node->next)
	{
		s_modifier *mod = ( s_modifier *) node->data;
		modifier_free( mod);
	}

	lnode_free( stone->node);

	llist_free( stone->vertex);
	llist_free( stone->edge);
	llist_free( stone->face);
	llist_free( stone->modifier);

	free( stone);
}

// Show

void stone_vertex_show( s_vertex *v)
{
	printf("Vertex %d [%f %f %f]\n", v->indice, v->co[0], v->co[1], v->co[2]);
}

void stone_face_show( s_face *f)
{
	printf("Face [%d %d %d %d]\n", f->a->indice, f->b->indice, f->c->indice, f->d->indice);
}

void stone_edge_show( s_edge *e)
{
	printf("Edge [%d %d]\n", e->a->indice, e->b->indice);
}

void stone_show_vertex( void *d)
{
	s_vertex *s = ( s_vertex *) d;
	stone_vertex_show( s);
}

void stone_show_face( void *d)
{
	s_face *f = ( s_face *) d;
	stone_face_show( f);
}

void stone_show_edge( void *d)
{
	s_edge *e = ( s_edge *) d;
	stone_edge_show( e);
}

void stone_show( t_stone *stone)
{
	printf("---------------------\n");
	printf("[STONE] %s\n", stone->name);
	printf("Vertex: %d\n", stone->vertex_count);
	llist_show_by_func( stone->vertex, stone_show_vertex);
	printf("Face: %d\n", stone->face_count);
	llist_show_by_func( stone->face, stone_show_face);
	printf("Edge: %d\n", stone->edge_count);
	llist_show_by_func( stone->edge, stone_show_edge);
	printf("---------------------\n");

	if( stone->node->next) 
	{
		printf("NEXT\n");
		stone_show( stone->node->next->data);
	}
}

// Add

s_vertex *stone_add_vertex_v( t_stone *stone, float *vec)
{
	s_vertex *s = stone_vertex_new( stone->vertex_count, vec);
	llist_push_front( stone->vertex, s);
	stone->vertex_count++;
	return s;
}

s_vertex *stone_add_vertex( t_stone *stone, float x, float y, float z)
{
	float p[3] = { x, y, z};
	return stone_add_vertex_v( stone, p);
}

s_face *stone_add_face( t_stone *stone, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	s_face *f = stone_face_new( a, b, c, d);
	llist_push_front( stone->face, f);
	stone->face_count++;
	return f;
}

s_edge *stone_add_edge( t_stone *stone, s_vertex *a, s_vertex *b)
{
	s_edge *e = stone_edge_new( a, b);
	llist_push_front( stone->edge, e);
	stone->edge_count++;
	return e;
}

void stone_add_next( t_stone *this, t_stone *next)
{
	lnode_add_next( this->node, next->node);
}

// Get

float *stone_get_vertex_buffer( t_stone *stone)
{
	float *v = ( float *) malloc( sizeof(float) * stone->vertex_count * 3);
	int j = 0;
	s_vertex *p;
	t_lnode *node;
	for( node = stone->vertex->first; node; node = node->next)
	{
		p = ( s_vertex *) node->data;
		v[j] = p->co[0];
		v[j+1] = p->co[1];
		v[j+2] = p->co[2];
		j+=3;
	}
	return v;
}

int stone_get_quad_count( t_stone *stone)
{
	int count = 0;
	s_face *face;
	t_lnode *node;
	for( node = stone->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		if( face->d) count++; 
	}
	return count;
}

int stone_get_tri_count( t_stone *stone)
{
	int count = 0;
	s_face *face;
	t_lnode *node;
	for( node = stone->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		if( !face->d) count++; 
	}
	return count;
}

int *stone_get_quad_buffer( t_stone *stone, int count)
{
	int *f = NULL;
	s_face *face;
	t_lnode *node;

	if( count != 0)
	{
		f = ( int *) malloc( sizeof(int) * count * 4);
		int j = 0;

		for( node = stone->face->first; node; node = node->next)
		{
			face = ( s_face *) node->data;

			if( face->d)
			{
				assert(face->a);
				assert(face->b);
				assert(face->c);
				assert(face->d);

				f[j+0] = face->a->indice;
				f[j+1] = face->b->indice;
				f[j+2] = face->c->indice;
				f[j+3] = face->d->indice;

				j+=4;
			}
		}
	}

	return f;
}

int *stone_get_tri_buffer( t_stone *stone, int count)
{
	int *f = NULL;
	s_face *face;
	t_lnode *node;

	if( count != 0)
	{
		f = ( int *) malloc( sizeof(int) * count * 3);
		int j = 0;

		// Make buffer
		for( node = stone->face->first; node; node = node->next)
		{
			face = ( s_face *) node->data;

			if( !face->d)
			{
				assert(face->a);
				assert(face->b);
				assert(face->c);

				f[j+0] = face->a->indice;
				f[j+1] = face->b->indice;
				f[j+2] = face->c->indice;

				j+=3;
			}
		}
	}

	return f;
}

int *stone_get_edge_buffer( t_stone *stone)
{
	if( stone->edge_count > 0)
	{
		int *e = ( int *) malloc( sizeof(int) * stone->edge_count * 2);
		int j = 0;
		s_edge *edge;
		t_lnode *node;
		for( node = stone->edge->first; node; node = node->next)
		{
			edge = ( s_edge *) node->data;

			assert(edge->a);
			assert(edge->b);

			e[j+0] = edge->a->indice;
			e[j+1] = edge->b->indice;

			j+=2;
		}

		return e;
	}
	else
	{
		return NULL;
	}
}

// Extrude

/**
 * @anchor extrude_vertex
 * @ref extrude_vertex
 * @fn s_vertex *_stone_extrude_vertex( t_stone *stone, s_vertex *vertex, float *v)
 * @brief Extrude a single vertex
 *
 */
s_vertex *_stone_extrude_vertex( t_stone *stone, s_vertex *vertex, float *v)
{
	float extrude[3];
	copy_v3_v3( extrude, vertex->co);
	add_v3_v3( extrude, v);

	s_vertex *p2 = stone_add_vertex_v( stone, extrude);
	return p2;
}

/**
 *
 * @brief Extrude a single vertex
 * @param[in] v (x,y,z) Vector 
 */
s_edge *stone_extrude_vertex( t_stone *stone, s_vertex *p1, float *v)
{
	s_vertex *p2 = _stone_extrude_vertex( stone, p1, v);
	s_edge *e = stone_add_edge( stone, p1, p2);
	return e;
}

s_face *stone_extrude_edge( t_stone *stone, s_edge *e, float *v)
{
	s_vertex *v3 = _stone_extrude_vertex( stone, e->a, v);
	s_vertex *v4 = _stone_extrude_vertex( stone, e->b, v);
	s_face *face = stone_add_face( stone, e->a, e->b, v4, v3);

	return face;
}

s_face *stone_extrude_face( t_stone *stone, s_face *face, float *v)
{
	s_vertex *a = face->a;
	s_vertex *b = face->b;
	s_vertex *c = face->c;
	s_vertex *d = face->d;

	s_edge *ea = stone_extrude_vertex( stone, a, v);
	s_edge *eb = stone_extrude_vertex( stone, b, v);
	s_edge *ec = stone_extrude_vertex( stone, c, v);
	s_edge *ed = stone_extrude_vertex( stone, d, v);

	s_vertex *aa = ea->b;
	s_vertex *bb = eb->b;
	s_vertex *cc = ec->b;
	s_vertex *dd = ed->b;

	stone_add_face( stone, a, b, bb, aa);
	stone_add_face( stone, b, c, cc, bb);
	stone_add_face( stone, c, d, dd, cc);
	stone_add_face( stone, d, a, aa, dd);

	s_face *f = stone_add_face( stone, aa, bb, cc, dd);

	return f;
}

// Build

void stone_build_circle( t_stone *stone, int resolution, float radius)
{
	float center[3] = { 0, 0, 0};
	float *points = std_circle( center, (double)radius, resolution);
	int i;
	int j=0;
	for ( i = 0; i < resolution; i++)
	{
		stone_add_vertex( stone, points[j], points[j+1], points[j+2]);
		j += 3;
	}
	free( points);
}

// Transform

void stone_apply_vertex( t_stone *stone, void (* f)( s_vertex *v, void *d), void *data)
{
	t_lnode *node;
	for( node = stone->vertex->first; node; node = node->next)
	{
		s_vertex *v = ( s_vertex *) node->data;
		f( v, data);
	}
}

void stone_translate_f( s_vertex *v, void *d)
{
	float *vec = ( float *) d;
	add_v3_v3( v->co, vec);
}

void stone_translate( t_stone *stone, float *v)
{
	stone_apply_vertex( stone, stone_translate_f, (void *) v);
}

// Merge

void stone_insert_vertex( t_stone *stone, s_vertex *v, int offset)
{
	v->indice += offset;
	llist_push_front( stone->vertex, v);
	stone->vertex_count++;
}

void stone_merge_points( t_stone *stone, t_stone *root)
{
	s_vertex *point;
	t_lnode *node;
	int offset = root->vertex_count;
	for( node = stone->vertex->first; node; node = node->next)
	{
		point = ( s_vertex *) node->data;
		assert( point != NULL);
		stone_insert_vertex( root, point, offset);
	}
}

void stone_merge_faces( t_stone *stone, t_stone *root)
{
	s_face *face;
	t_lnode *node;
	for( node = stone->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		assert( face != NULL);
		stone_add_face( root, face->a, face->b, face->c, face->d);
	}
}

void stone_merge( t_stone *stone, t_stone *root)
{
	// Merge next
	if( stone->node->next)
	{
		// Prevent auto-references
		if( stone->node->next == stone->node)
		{
			printf("[stone] Error, cannot merge self\n");
			return;
		}
		if( root ) stone_merge( stone->node->next->data, root);
		else stone_merge( stone->node->next->data, stone); // first pass, set this stone as root
	}

	// No root means first pass
	// Merge faces first -> vertex count id
	stone_apply_modifiers( stone);
	if( root) stone_merge_faces( stone, root); 
	if( root) stone_merge_points( stone, root);
}

// Copy

// Copy vertices: create new vertices from src 
void stone_copy_points( t_stone *stone, t_stone *src)
{
	s_vertex *point;
	t_lnode *node;
	for( node = src->vertex->first; node; node = node->next)
	{
		point = ( s_vertex *) node->data;
		assert( point != NULL);
		stone_add_vertex( stone, point->co[0], point->co[1], point->co[2]);
	}
}


// Copy faces: add new face with updated vertex pointers
void stone_copy_faces( t_stone *stone, t_stone *src)
{
	s_face *face;
	t_lnode *node;
	for( node = src->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		assert( face != NULL);
		s_vertex *a = stone_get_vertex_by_indice( stone, face->a->indice);
		s_vertex *b = stone_get_vertex_by_indice( stone, face->b->indice);
		s_vertex *c = stone_get_vertex_by_indice( stone, face->c->indice);
		s_vertex *d = stone_get_vertex_by_indice( stone, face->d->indice);
		stone_add_face( stone, a, b, c, d);
	}
}

t_stone *stone_copy( t_stone *stone)
{
	t_stone *new = stone_new( stone->name);
	stone_copy_points( new, stone);
	stone_copy_faces( new, stone);
	return new;
}
