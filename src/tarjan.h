//
// Created by mouro on 20/11/2025.
//

#ifndef TARJAN_H
#define TARJAN_H
#include "graphe.h"

typedef struct {
    int id;
    int index;
    int lowlink;
    int onStack;
} t_tarjan_vertex;

typedef struct {
    int *sommets;
    int nb;
    char name[10];
} t_classe;

typedef struct {
    t_classe *classes;
    int nbClasses;
} t_partition;

t_tarjan_vertex *init_tarjan_vertices(liste_adjacence g);

typedef struct {
    int *data;
    int top;
    int capacity;
} t_stack;

t_stack *stack_create(int capacity);
void stack_push(t_stack *s, int v);
int stack_pop(t_stack *s);
int stack_peek(t_stack *s);
int stack_is_empty(const t_stack *s);
void stack_free(t_stack *s);

t_partition tarjan(liste_adjacence g);

void free_partition(t_partition *p);

#endif
