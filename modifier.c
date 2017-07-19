/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "modifier.h"
#include "mud.h"
#include "llist.h"

static int mod_debug = 0;

void set_max_name( char *dst, const char *src)
{
	strncpy(dst,src,MAX_NAME_LENGTH);
	dst[MAX_NAME_LENGTH-1]='\0';
}

s_modifier *modifier_new( const char *name, void *data, void (* f)( t_mud *mud, s_modifier *mod))
{
	s_modifier *mod = ( s_modifier *) calloc( 1, sizeof( s_modifier));
	mod->data = data;
	mod->f = f;
	set_max_name( mod->name, name);
	return mod;
}

void modifier_free( s_modifier *mod)
{
	free( mod->data);
	free( mod);
}

void mud_add_modifier( t_mud *mud, s_modifier *mod)
{
	llist_push_front( mud->modifier, mod);
}

void mud_apply_modifiers( t_mud *mud)
{
	t_lnode *node;
	for( node = mud->modifier->first; node; node = node->next)
	{
		s_modifier *mod = ( s_modifier *) node->data;
		if( !mod->done)
		{
			mod->done = 1;
			mod->f( mud, mod);
			if( mod_debug) printf("[Apply Modifier] <%s>\n", mod->name);
		}
	}
}
