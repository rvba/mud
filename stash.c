/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "stash.h"

t_stash *stash_new( size_t size)
{
	t_stash *stash = ( t_stash *) calloc( 1, sizeof( t_stash));
	stash->size = size;
	return stash;
}

void stash_inflate( t_stash *stash, int count)
{
	stash->data = ( unsigned char *) realloc( stash->data, stash->size * ( count + stash->count ));
	stash->count += count;
}

void stash_add( t_stash *stash, const void *data)
{
	if( stash->next >= stash->count)
	{
		stash_inflate( stash, 100);
	}

	int i;
	int pos = stash->next * stash->size;
	unsigned char *d = ( unsigned char *) data;
	assert( data != NULL);
	for( i = 0; i < stash->size; i++)
	{
		stash->data[ pos+i] = d[i];
	}
	stash->next++;
}

void stash_show( t_stash *stash, const char *type)
{
	int i;
	unsigned char *d = stash->data;
	printf("Stash type:%s\n", type);
	for( i = 0; i < stash->next; i++)
	{
		printf("%d) ", i);
		if( strcmp( type, "float") == 0)
		{
			int count = stash->size / sizeof( float);
			int k;
			for( k = 0; k < count; k++)
			{
				printf("%f ", *( float *)d);
				d+= sizeof( float);
			}
		}
		else if( strcmp( type, "int") == 0)
		{
			int count = stash->size / sizeof( int);
			int k;
			for( k = 0; k < count; k++)
			{
				printf("%d ", *( int *)d);
				d+= sizeof( int);
			}
		}
		else if( strcmp( type, "string") == 0)
		{
			int count = stash->size;
			printf("%s", (char *) d);
			d += count;
		}
		else
		{
			printf("Unknown type %s\n", type);

		}
		printf("\n");
	}
}

void stash_show_by_func( t_stash *stash, void (* f)( void *d))
{
	int i;
	for( i = 0; i < stash->next; i++)
	{
		void *d = stash_get_elem( stash, i);
		f( d);
	}
}

void *stash_get_elem( t_stash *stash, int index)
{
	if( index > stash->next)
	{
		return NULL;
	}
	else
	{
		return stash->data + (index * stash->size);
	}
}

void stash_free( t_stash *stash)
{
	free(stash->data);
	free( stash);
}
