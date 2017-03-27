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
 *  [**Project wiki page**](http://rvba.fr/waki/doku.php?id=code:dev:minuit:modules:stone)
 *
 * A Stone object is composed of lists of:
 * - SVertex
 * - SEdge
 * - SFace
 *
 * Operators:
 *
 * - [extrude_vertex](@ref stone_extrude_vertex)
 * - [extrude_edge](@ref stone_extrude_edge)
 *
 */


#ifndef __STONE_H__
#define __STONE_H__

#define STONE_NAME 32

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
	char name[STONE_NAME];
	struct LNode *node;

	struct LList *vertex;
	int vertex_count;

	struct LList *face; // Only quads
	int face_count;

	struct LList *edge;
	int edge_count;

	struct LList *modifier;
	int modifier_count;

}t_stone;

void stone_vertex_update( t_stone *stone, int indice, float *vector);
s_vertex *stone_add_vertex_v( t_stone *stone, float *vec);
s_vertex *stone_add_vertex( t_stone *stone, float x, float y, float z);
int stone_add_face_v( t_stone *stone, int *f);
s_face *stone_add_face( t_stone *stone, s_vertex *a, s_vertex *b, s_vertex *c, s_vertex *d);
s_edge *stone_add_edge( t_stone *stone, s_vertex *a, s_vertex *b);
void stone_add_next( t_stone *this, t_stone *next);

s_edge *stone_extrude_vertex( t_stone *stone, s_vertex *p1, float *v);
s_face *stone_extrude_edge( t_stone *stone, s_edge *edge, float *v);
s_face *stone_extrude_face( t_stone *stone, s_face *face, float *v);

void stone_build_circle( t_stone *stone, int resolution, float radius);

void stone_translate( t_stone *stone, float *v);

void stone_apply_vertex( t_stone *stone, void (* f)( s_vertex *v, void *d), void *data);

float *stone_get_vertex_buffer( t_stone *stone);
int stone_get_quad_count( t_stone *stone);
int stone_get_tri_count( t_stone *stone);
int *stone_get_quad_buffer( t_stone *stone, int count);
int *stone_get_tri_buffer( t_stone *stone, int count);
int *stone_get_edge_buffer( t_stone *stone);
float *stone_get_vertex_pointer( t_stone *stone);
s_vertex *stone_get_vertex( t_stone *stone, int indice);
s_face *stone_get_face( t_stone *stone, int indice);

void stone_show( t_stone *stone);
void stone_vertex_show( s_vertex *v);
void stone_face_show( s_face *f);
void stone_edge_show( s_edge *e);

void stone_free( t_stone *stone);
t_stone *stone_new( const char *name);
void stone_merge( t_stone *stone, t_stone *root);
void stone_module_init( struct Context *C);

t_stone *stone_copy( t_stone *stucco);

#endif
