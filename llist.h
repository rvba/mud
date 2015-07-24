/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __LLIST_H__
#define __LLIST_H__

typedef struct LNode
{
	struct LNode *next;
	struct LNode *prev;

	void *data;

}t_lnode;


typedef struct LList
{
	t_lnode *first;
	t_lnode *last;

}t_llist;

/* LLIST */

t_llist *llist_new( void);
void llist_free( t_llist *list);
t_lnode *llist_push_front( t_llist *list, void *data);
void llist_show_by_func( t_llist *list, void (* f)( void *d));
t_lnode *llist_get_lnode_by_indice( t_llist *list, int indice);

/* LNODE */

void lnode_add_prev( t_lnode *this_node, t_lnode *prev_node);
void lnode_add_next( t_lnode *this_node, t_lnode *next_node);
void lnode_free( t_lnode *node);
t_lnode *lnode_new( void *data);

#endif
