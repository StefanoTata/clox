#ifndef CONTAINERS_H
#define CONTAINERS_H

typedef struct List{
  struct List *next;
} List;

#define list_for_each(pos, node)\
  for (__typeof__(*(node)) *(pos) = (node), *n; (pos) && (n = (pos)->next, 1); (pos) = n)

#define add_node(prv_node, new_node) (prv_node)->list.next = &(new_node)->list;
#define get_node(el, node) (void*)((char*)(el) - (offsetof(node, list)));

#endif CONTAINERS_H
