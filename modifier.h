/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __MODIFIER_H__
#define __MODIFIER_H__

#define MAX_NAME_LENGTH 128

struct Stone;
struct SModifier;
struct MN_MAT3;
struct MN_MAT4;

typedef struct SModifier
{
	char name[MAX_NAME_LENGTH];
	void *data;
	void (* f)( struct Stone *stone, struct SModifier *mod);
	int done;

}s_modifier;

s_modifier *modifier_new( const char *name, void *data, void (* f)( struct Stone *stone, s_modifier *mod));
void modifier_free( s_modifier *mod);
void stone_add_modifier( struct Stone *stone, s_modifier *mod);
void stone_apply_modifiers( struct Stone *stone);

void stone_add_modifier_matrix( struct Stone *stone, struct MN_MAT4 *mat);
void stone_add_modifier_skin( struct Stone *stone);
void stone_add_modifier_rotation( struct Stone *stone, const float v[3], const float a);
void stone_add_modifier_array( struct Stone *stone, int count, struct MN_MAT4 *mat);

#endif
