/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "llist.h"

// LNODE

void lnode_add_prev( t_lnode *this_node, t_lnode *prev_node)
{
	if( this_node->prev)
	{
		this_node->prev->next = NULL;
		lnode_add_next( this_node->prev, prev_node);
	}
	else
	{
		/* prev_node next should be empty */
		assert( prev_node->next == NULL);
		this_node->prev = prev_node;
		prev_node->next = this_node;
	}
}

void lnode_add_next( t_lnode *this_node, t_lnode *next_node)
{
	if( this_node->next)
	{
		this_node->next->prev = NULL;
		lnode_add_prev( this_node->next, next_node);
	}
	else
	{
		/* Next_node prev should be empty */
		assert( next_node->prev == NULL);

		this_node->next = next_node;
		next_node->prev = this_node;
	}
}

void lnode_free( t_lnode *node)
{
	free( node);
}

t_lnode *lnode_new( void *data)
{
	t_lnode *node = ( t_lnode *) calloc( 1, sizeof( t_lnode));
	node->data = data;
	return node;
}

// LLIST

t_lnode *llist_get_lnode_by_indice( t_llist *list, int indice)
{
	t_lnode *node;
	int i = 0;
	for( node = list->first; node; node = node->next)
	{
		if( i == indice) return node;
		i++;
	}
	return NULL;
}

t_lnode *llist_push_front( t_llist *list, void *data)
{
	t_lnode *node = lnode_new( data);

	if( list->first)
	{
		lnode_add_next( list->last, node);
		list->last = node;
	}
	else
	{
		list->first = node;
		list->last = node;
	}

	return node;
}

t_llist *llist_new( void)
{
	t_llist *list = ( t_llist *) calloc( 1, sizeof( t_llist));
	return list;
}

void llist_free( t_llist *list)
{
	t_lnode *node = list->first;
	t_lnode *tmp = NULL;

	while( node)
	{
		tmp = node->next;
		lnode_free( node);
		node = tmp;
	}

	free( list);
}

void llist_show_by_func( t_llist *list, void (* f)( void *d))
{
	t_lnode *node;
	for( node = list->first; node; node = node->next)
	{
		void *d = node->data;
		f( d);
	}
}



