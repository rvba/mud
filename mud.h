/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

/**
 * @mainpage
 * Stone
 * =====
 *
 * A very simple mesh modeling toolkit.
 *
 * Intro
 * -----
 *  [**Project wiki page**](http://rvba.fr/waki/doku.php?id=code:dev:minuit:modules:mud)
 *
 * A Stone object is composed of lists of:
 * - SVertex
 * - SEdge
 * - SFace
 *
 * Operators:
 *
 * - [extrude_vertex](@ref mud_extrude_vertex)
 * - [extrude_edge](@ref mud_extrude_edge)
 *
 */


#ifndef __MUD_H__
#define __MUD_H__

#define MUD_NAME 32
#define MUD_FACE_EXISTS 1
#define MUD_FACE_DELETED 2

struct Stash;
struct Stone;
struct Context;

/**
 * @brief A single vertex.
 */
typedef struct SVertex
{
	int indice; /**< Indice (ID) of the vertex (Stored position in the list) */
	float co[3]; /**<  Coordinates (x,y,z) of the vertex */

}s_vertex;

/**
 * A single Face.
 *
 * Quad: four SVertex (a,b,c,d)
 * Tri: three vertices (d is NULL)
 */
typedef struct SFace
{
	s_vertex *a;
	s_vertex *b;
	s_vertex *c;
	s_vertex *d;
	short status;
}s_face;

typedef struct SEdge
{
	s_vertex *a;
	s_vertex *b;
}s_edge;

/**
 * @ref Stone
 * @brief Main Object.
 *
 * Stores lists of
 *
 *- [vertex](@ref SVertex)
 *- edges
 *- faces
 */
typedef struct Stone 
{
	char name[MUD_NAME];
	struct LNode *node;

	struct LList *vertex;
	int vertex_count;

	struct LList *face; // Only quads
	int face_count;

	struct LList *edge;
	int edge_count;

	struct LList *modifier;
	int modifier_count;

}t_mud;

void mud_vertex_update( t_mud *mud, int indice, float *vector);
s_vertex *mud_add_vertex_v( t_mud *mud, float *vec);
s_vertex *mud_add_vertex( t_mud *mud, float x, float y, float z);
int mud_add_face_v( t_mud *mud, int *f);
s_face *mud_add_face( t_mud *mud, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d);
s_edge *mud_add_edge( t_mud *mud, s_vertex *a, s_vertex *b);
void mud_add_next( t_mud *this, t_mud *next);

void mud_delete_face( t_mud *mud, s_face *face);

s_edge *mud_extrude_vertex( t_mud *mud, s_vertex *p1, float *v);
s_face *mud_extrude_edge( t_mud *mud, s_edge *edge, float *v);
s_face *mud_extrude_face( t_mud *mud, s_face *face, float *v);

void mud_translate( t_mud *mud, float *v);
void mud_apply_vertex( t_mud *mud, void (* f)( s_vertex *v, void *d), void *data);

float *mud_get_vertex_buffer( t_mud *mud);
int mud_get_quad_count( t_mud *mud);
int mud_get_tri_count( t_mud *mud);
int *mud_get_quad_buffer( t_mud *mud, int count);
int *mud_get_tri_buffer( t_mud *mud, int count);
int *mud_get_edge_buffer( t_mud *mud);
float *mud_get_vertex_pointer( t_mud *mud);
s_vertex *mud_get_vertex( t_mud *mud, int indice);
s_face *mud_get_face( t_mud *mud, int indice);

void mud_show( t_mud *mud);
void mud_vertex_show( s_vertex *v);
void mud_face_show( s_face *f);
void mud_edge_show( s_edge *e);

void mud_free( t_mud *mud);
t_mud *mud_new( const char *name);
void mud_merge( t_mud *mud, t_mud *root);
void mud_module_init( struct Context *C);

s_vertex *mud_copy_vertex( s_vertex *v);
t_mud *mud_copy( t_mud *stucco);
t_mud *mud_copy_with_name( t_mud *mud, const char *name);

void mud_tessellate( t_mud *mud);
void mud_tess_debug( int val);
void mud_tess_set( const char *what, void *data);
#endif
