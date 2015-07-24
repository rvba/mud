/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __STASH_H__
#define __STASH_H__

#include <stddef.h>

typedef struct Stash
{
	size_t size;
	unsigned int count;
	unsigned int next;
	unsigned char *data;

}t_stash;

t_stash *stash_new( size_t elem_size);
void stash_add( t_stash *stash, const void *data);
void stash_show( t_stash *stash, const char *type);
void stash_show_by_func( t_stash *stash, void (* f)( void *d));
void *stash_get_elem( t_stash *stash, int index);
void stash_free( t_stash *stash);


#endif
