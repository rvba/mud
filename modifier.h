/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __MODIFIER_H__
#define __MODIFIER_H__

#define MAX_NAME_LENGTH 128

struct Mud;
struct SModifier;
struct MN_MAT3;
struct MN_MAT4;

typedef struct SModifier
{
	char name[MAX_NAME_LENGTH];
	void *data;
	void (* f)( struct Mud *mud, struct SModifier *mod);
	int done;

}s_modifier;

s_modifier *modifier_new( const char *name, void *data, void (* f)( struct Mud *mud, s_modifier *mod));
void modifier_free( s_modifier *mod);
void mud_add_modifier( struct Mud *mud, s_modifier *mod);
void mud_apply_modifiers( struct Mud *mud);

void mud_add_modifier_matrix( struct Mud *mud, struct MN_MAT4 *mat);
void mud_add_modifier_skin( struct Mud *mud);
void mud_add_modifier_rotation( struct Mud *mud, const float v[3], const float a);
void mud_add_modifier_array( struct Mud *mud, int count, struct MN_MAT4 *mat);

#endif
