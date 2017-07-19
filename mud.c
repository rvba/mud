/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

/**
 * @file Main Mud file
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mud.h"
#include "modifier.h"
#include "stash.h"
#include "llist.h"

#include "blenlib.h"
#include "stdmath.h"
#include "base.h"

static short debug_merge = 0;

// New

/**
 * @ref mud_vertex_set
 * @fn void mud_vertex_set( s_vertex *vert, float *v)
 * set x y z
 */
void mud_vertex_set( s_vertex *vert, float *v)
{
	vert->co[0] = v[0];
	vert->co[1] = v[1];
	vert->co[2] = v[2];
}

void mud_face_set( s_face *face, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	face->a = a;
	face->b = b;
	face->c = c;
	face->d = d;
}

s_vertex *mud_vertex_new( int indice, float *v)
{
	s_vertex *vert = ( s_vertex *) calloc( 1, sizeof( s_vertex));
	vert->indice = indice;
	mud_vertex_set( vert, v);
	return vert;
}

s_edge *mud_edge_new( s_vertex *a, s_vertex *b)
{
	s_edge *edge = ( s_edge *) calloc( 1, sizeof( s_edge));
	edge->a = a;
	edge->b = b;
	return edge;
}

s_face *mud_face_new( s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	s_face *face = ( s_face *) calloc( 1, sizeof( s_face));
	mud_face_set( face, a, b, c, d);
	face->status = MUD_FACE_EXISTS;
	return face;
}

t_mud *mud_new( const char *name)
{
	t_mud *mud = ( t_mud *) calloc( 1, sizeof( t_mud));
	strncpy( mud->name, name, MUD_NAME);
	mud->node = lnode_new( mud);
	mud->vertex = llist_new();
	mud->face = llist_new();
	mud->edge = llist_new();
	mud->modifier = llist_new();
	return mud;
}

/* Get */

s_vertex *mud_get_vertex_by_indice( t_mud *mud, int indice)
{
	t_lnode *node = llist_get_lnode_by_indice( mud->vertex, indice);
	assert( node);
	return ( s_vertex *) node->data;
}

s_face *mud_get_face( t_mud *mud, int indice)
{
	t_lnode *node = llist_get_lnode_by_indice( mud->face, indice);
	assert( node);
	return ( s_face *) node->data;
}

s_vertex *mud_get_vertex( t_mud *mud, int indice)
{
	return mud_get_vertex_by_indice( mud, indice);
}

/* Set */

void mud_vertex_update( t_mud *mud, int indice, float *vector)
{
	s_vertex *vertex = mud_get_vertex( mud, indice);
	if( vertex) mud_vertex_set( vertex, vector);
	else printf("[mud] Error, can't get vertex %d\n", indice);
}

// Free

void mud_vertex_free( s_vertex *v)
{
	free(v);
}

void mud_edge_free( s_edge *e)
{
	free(e);
}

void mud_face_free( s_face *f)
{
	free(f);
}

void mud_free( t_mud *mud)
{
	t_lnode *node;

	for( node = mud->vertex->first; node; node = node->next)
	{
		s_vertex *v = ( s_vertex *) node->data;
		mud_vertex_free( v);
	}

	for( node = mud->edge->first; node; node = node->next)
	{
		s_edge *e = ( s_edge *) node->data;
		mud_edge_free( e);
	}

	for( node = mud->face->first; node; node = node->next)
	{
		s_face *f = ( s_face *) node->data;
		mud_face_free( f);
	}

	for( node = mud->modifier->first; node; node = node->next)
	{
		s_modifier *mod = ( s_modifier *) node->data;
		modifier_free( mod);
	}

	lnode_free( mud->node);

	llist_free( mud->vertex);
	llist_free( mud->edge);
	llist_free( mud->face);
	llist_free( mud->modifier);

	free( mud);
}

// Show

static int print_tri_count = 0;
static int print_quad_count = 0;

void mud_vertex_show( s_vertex *v)
{
	printf("Vertex %d [%f %f %f]\n", v->indice, v->co[0], v->co[1], v->co[2]);
}

void mud_face_show( s_face *f)
{
	if(f->d)
	{
		printf("Face Quad %d [%d %d %d %d]\n", print_quad_count, f->a->indice, f->b->indice, f->c->indice, f->d->indice);
		print_quad_count++;
	}
	else
	{
		printf("Face Tri %d [%d %d %d]\n", print_tri_count, f->a->indice, f->b->indice, f->c->indice);
		print_tri_count++;
	}
}

void mud_edge_show( s_edge *e)
{
	printf("Edge [%d %d]\n", e->a->indice, e->b->indice);
}

void mud_show_vertex( void *d)
{
	s_vertex *s = ( s_vertex *) d;
	mud_vertex_show( s);
}

void mud_show_face( void *d)
{
	s_face *f = ( s_face *) d;
	mud_face_show( f);
}

void mud_show_edge( void *d)
{
	s_edge *e = ( s_edge *) d;
	mud_edge_show( e);
}

void mud_show( t_mud *mud)
{
	print_tri_count = 0;
	print_quad_count = 0;
	printf("---------------------\n");
	printf("[MUD] %s\n", mud->name);
	printf("Vertex: %d\n", mud->vertex_count);
	llist_show_by_func( mud->vertex, mud_show_vertex);
	printf("Face: %d\n", mud->face_count);
	llist_show_by_func( mud->face, mud_show_face);
	printf("Edge: %d\n", mud->edge_count);
	llist_show_by_func( mud->edge, mud_show_edge);
	printf("---------------------\n");

	if( mud->node->next) 
	{
		printf("NEXT\n");
		mud_show( mud->node->next->data);
	}
}

// Add

s_vertex *mud_add_vertex_v( t_mud *mud, float *vec)
{
	s_vertex *s = mud_vertex_new( mud->vertex_count, vec);
	llist_push_front( mud->vertex, s);
	mud->vertex_count++;
	return s;
}

s_vertex *mud_add_vertex( t_mud *mud, float x, float y, float z)
{
	float p[3] = { x, y, z};
	return mud_add_vertex_v( mud, p);
}

s_face *mud_add_face( t_mud *mud, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d)
{
	s_face *f = mud_face_new( a, b, c, d);
	llist_push_front( mud->face, f);
	mud->face_count++;
	return f;
}

s_edge *mud_add_edge( t_mud *mud, s_vertex *a, s_vertex *b)
{
	s_edge *e = mud_edge_new( a, b);
	llist_push_front( mud->edge, e);
	mud->edge_count++;
	return e;
}

void mud_add_next( t_mud *this, t_mud *next)
{
	lnode_add_next( this->node, next->node);
}

// Delete


/* TODO: use this deleted face to be filled with new faces
 * TODO: add a resize function to get rid off unused faces
 */
void mud_delete_face( t_mud *mud, s_face *face)
{
	face->status = MUD_FACE_DELETED;
}

// Get

float *mud_get_vertex_buffer( t_mud *mud)
{
	float *v = ( float *) malloc( sizeof(float) * mud->vertex_count * 3);
	int j = 0;
	s_vertex *p;
	t_lnode *node;
	for( node = mud->vertex->first; node; node = node->next)
	{
		p = ( s_vertex *) node->data;
		v[j] = p->co[0];
		v[j+1] = p->co[1];
		v[j+2] = p->co[2];
		j+=3;
	}
	return v;
}

int mud_get_quad_count( t_mud *mud)
{
	int count = 0;
	s_face *face;
	t_lnode *node;
	for( node = mud->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		if( face->d && face->status == MUD_FACE_EXISTS) count++; 
	}
	return count;
}

int mud_get_tri_count( t_mud *mud)
{
	int count = 0;
	s_face *face;
	t_lnode *node;
	for( node = mud->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		if( !face->d && face->status == MUD_FACE_EXISTS) count++; 
	}
	return count;
}

int *mud_get_quad_buffer( t_mud *mud, int count)
{
	int *f = NULL;
	s_face *face;
	t_lnode *node;

	if( count != 0)
	{
		f = ( int *) malloc( sizeof(int) * count * 4);
		int j = 0;

		for( node = mud->face->first; node; node = node->next)
		{
			face = ( s_face *) node->data;

			if( face->d && face->status == MUD_FACE_EXISTS)
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

int *mud_get_tri_buffer( t_mud *mud, int count)
{
	int *f = NULL;
	s_face *face;
	t_lnode *node;

	if( count != 0)
	{
		f = ( int *) malloc( sizeof(int) * count * 3);
		int j = 0;

		// Make buffer
		for( node = mud->face->first; node; node = node->next)
		{
			face = ( s_face *) node->data;

			if( !face->d && face->status == MUD_FACE_EXISTS)
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

int *mud_get_edge_buffer( t_mud *mud)
{
	if( mud->edge_count > 0)
	{
		int *e = ( int *) malloc( sizeof(int) * mud->edge_count * 2);
		int j = 0;
		s_edge *edge;
		t_lnode *node;
		for( node = mud->edge->first; node; node = node->next)
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
 * @fn s_vertex *_mud_extrude_vertex( t_mud *mud, s_vertex *vertex, float *v)
 * @brief Extrude a single vertex
 *
 */
s_vertex *_mud_extrude_vertex( t_mud *mud, s_vertex *vertex, float *v)
{
	float extrude[3];
	copy_v3_v3( extrude, vertex->co);
	add_v3_v3( extrude, v);

	s_vertex *p2 = mud_add_vertex_v( mud, extrude);
	return p2;
}

/**
 *
 * @brief Extrude a single vertex
 * @param[in] v (x,y,z) Vector 
 */
s_edge *mud_extrude_vertex( t_mud *mud, s_vertex *p1, float *v)
{
	s_vertex *p2 = _mud_extrude_vertex( mud, p1, v);
	s_edge *e = mud_add_edge( mud, p1, p2);
	return e;
}

s_face *mud_extrude_edge( t_mud *mud, s_edge *e, float *v)
{
	s_vertex *v3 = _mud_extrude_vertex( mud, e->a, v);
	s_vertex *v4 = _mud_extrude_vertex( mud, e->b, v);
	s_face *face = mud_add_face( mud, e->a, e->b, v4, v3);

	return face;
}

s_face *mud_extrude_face( t_mud *mud, s_face *face, float *v)
{
	s_vertex *a = face->a;
	s_vertex *b = face->b;
	s_vertex *c = face->c;
	s_vertex *d = face->d;

	s_edge *ea = mud_extrude_vertex( mud, a, v);
	s_edge *eb = mud_extrude_vertex( mud, b, v);
	s_edge *ec = mud_extrude_vertex( mud, c, v);
	s_edge *ed = NULL;

	/* quad */
	if(d) ed = mud_extrude_vertex( mud, d, v);

	s_vertex *aa = ea->b;
	s_vertex *bb = eb->b;
	s_vertex *cc = ec->b;
	s_vertex *dd = NULL;

	/* quad */
	if(d) dd = ed->b;

	mud_add_face( mud, a, b, bb, aa);
	mud_add_face( mud, b, c, cc, bb);

	/* quad */
	if(d)
	{
		mud_add_face( mud, d, a, aa, dd);
		mud_add_face( mud, c, d, dd, cc);
	}	
	else
	{
		mud_add_face( mud, c, a, cc, aa);
	}

	/* quad */
	s_face *f = NULL;
	f = mud_add_face( mud, aa, bb, cc, dd);

	return f;
}

// Transform

void mud_apply_vertex( t_mud *mud, void (* f)( s_vertex *v, void *d), void *data)
{
	t_lnode *node;
	for( node = mud->vertex->first; node; node = node->next)
	{
		s_vertex *v = ( s_vertex *) node->data;
		f( v, data);
	}
}

void mud_translate_f( s_vertex *v, void *d)
{
	float *vec = ( float *) d;
	add_v3_v3( v->co, vec);
}

void mud_translate( t_mud *mud, float *v)
{
	mud_apply_vertex( mud, mud_translate_f, (void *) v);
}

// Merge

void mud_insert_vertex( t_mud *mud, s_vertex *v, int offset)
{
	v->indice += offset;
	llist_push_front( mud->vertex, v);
	if( debug_merge)
	{
		printf("[mud] merge offset: %d vertex ", offset);
		mud_vertex_show( v);
	}
	mud->vertex_count++;
}

void mud_merge_points( t_mud *mud, t_mud *root)
{
	s_vertex *point;
	t_lnode *node;
	int offset = root->vertex_count;
	for( node = mud->vertex->first; node; node = node->next)
	{
		point = ( s_vertex *) node->data;
		assert( point != NULL);
		mud_insert_vertex( root, point, offset);
	}
}

void mud_merge_faces( t_mud *mud, t_mud *root)
{
	s_face *face;
	t_lnode *node;
	for( node = mud->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		assert( face != NULL);
		mud_add_face( root, face->a, face->b, face->c, face->d);
	}
}

void mud_merge( t_mud *mud, t_mud *root)
{
	if( debug_merge)
	{
		if( root)
		{
			printf("[mud] merge\n");
			printf("[mud] root\n");
			mud_show(root);
			printf("[mud] target\n");
			mud_show(mud);
		}
	}
	// Merge next
	if( mud->node->next)
	{
		// Prevent auto-references
		if( mud->node->next == mud->node)
		{
			printf("[mud] Error, cannot merge self\n");
			return;
		}
		if( root ) mud_merge( mud->node->next->data, root);
		else mud_merge( mud->node->next->data, mud); // first pass, set this mud as root
	}

	// No root means first pass
	// Merge faces first -> vertex count id
	mud_apply_modifiers( mud);
	if( root) mud_merge_faces( mud, root); 
	if( root) mud_merge_points( mud, root);
}

// Copy a single "detached" vertex
s_vertex *mud_copy_vertex( s_vertex *v)
{
	return mud_vertex_new( 0, v->co);
}

// Copy vertices: create new vertices from src 
void mud_copy_points( t_mud *mud, t_mud *src)
{
	s_vertex *point;
	t_lnode *node;
	for( node = src->vertex->first; node; node = node->next)
	{
		point = ( s_vertex *) node->data;
		assert( point != NULL);
		mud_add_vertex( mud, point->co[0], point->co[1], point->co[2]);
	}
}


// Copy faces: add new face with updated vertex pointers
void mud_copy_faces( t_mud *mud, t_mud *src)
{
	s_face *face;
	t_lnode *node;
	for( node = src->face->first; node; node = node->next)
	{
		face = ( s_face *) node->data;
		assert( face != NULL);
		s_vertex *a = mud_get_vertex_by_indice( mud, face->a->indice);
		s_vertex *b = mud_get_vertex_by_indice( mud, face->b->indice);
		s_vertex *c = mud_get_vertex_by_indice( mud, face->c->indice);
		s_vertex *d = NULL;
		if(face->d) d = mud_get_vertex_by_indice( mud, face->d->indice);

		mud_add_face( mud, a, b, c, d);
	}
}

t_mud *mud_copy( t_mud *mud)
{
	t_mud *new = mud_new( mud->name);
	s_increment( new->name, MUD_NAME);
	mud_copy_points( new, mud);
	mud_copy_faces( new, mud);
	//new->node->next = mud->node->next;
	return new;
}

t_mud *mud_copy_with_name( t_mud *mud, const char *name)
{
	t_mud *new = mud_new( name);
	mud_copy_points( new, mud);
	mud_copy_faces( new, mud);
	//new->node->next = mud->node->next;
	return new;
}
