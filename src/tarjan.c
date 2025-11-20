#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tarjan.h"


t_tarjan_vertex *init_tarjan_vertices(liste_adjacence g) {
    t_tarjan_vertex *V = malloc(g.taille * sizeof(t_tarjan_vertex));
    if (!V) { perror("malloc"); exit(EXIT_FAILURE); }

    for (int i = 0; i < g.taille; i++) {
        V[i].id = i + 1;
        V[i].index = -1;
        V[i].lowlink = -1;
        V[i].onStack = 0;
    }
    return V;
}


t_stack *stack_create(int capacity) {
    t_stack *s = malloc(sizeof(t_stack));
    s->data = malloc(capacity * sizeof(int));
    s->top = -1;
    s->capacity = capacity;
    return s;
}

void stack_push(t_stack *s, int v) {
    s->data[++(s->top)] = v;
}

int stack_pop(t_stack *s) {
    return s->data[(s->top)--];
}

int stack_peek(t_stack *s) {
    return s->data[s->top];
}

int stack_is_empty(const t_stack *s) {
    return s->top < 0;
}

void stack_free(t_stack *s) {
    free(s->data);
    free(s);
}


static int global_index;
static t_stack *P;
static t_tarjan_vertex *TV;
static t_partition *Partition;
static liste_adjacence *G;

static void ajouter_sommet_classe(t_classe *C, int sommet) {
    C->sommets = realloc(C->sommets, (C->nb + 1) * sizeof(int));
    C->sommets[C->nb++] = sommet;
}

static void ajouter_classe(t_partition *P) {
    P->classes = realloc(P->classes, (P->nbClasses + 1) * sizeof(t_classe));
    t_classe *C = &P->classes[P->nbClasses];

    C->sommets = NULL;
    C->nb = 0;
    sprintf(C->name, "C%d", P->nbClasses + 1);

    P->nbClasses++;
}

static void parcours(int v) {
    TV[v].index = global_index;
    TV[v].lowlink = global_index;
    global_index++;

    stack_push(P, v);
    TV[v].onStack = 1;

    cellule *cur = G->tab[v].head;
    while (cur) {
        int w = cur->arrivee - 1;

        if (TV[w].index == -1) {
            parcours(w);
            TV[v].lowlink = (TV[v].lowlink < TV[w].lowlink)
                            ? TV[v].lowlink : TV[w].lowlink;
        }
        else if (TV[w].onStack) {
            TV[v].lowlink = (TV[v].lowlink < TV[w].index)
                            ? TV[v].lowlink : TV[w].index;
        }
        cur = cur->suiv;
    }

    if (TV[v].lowlink == TV[v].index) {
        ajouter_classe(Partition);
        t_classe *C = &Partition->classes[Partition->nbClasses - 1];

        int w;
        do {
            w = stack_pop(P);
            TV[w].onStack = 0;
            ajouter_sommet_classe(C, TV[w].id);
        } while (w != v);
    }
}

t_partition tarjan(liste_adjacence g) {
    t_partition partition;
    partition.classes = NULL;
    partition.nbClasses = 0;

    Partition = &partition;
    G = &g;

    TV = init_tarjan_vertices(g);

    P = stack_create(g.taille);
    global_index = 0;

    for (int v = 0; v < g.taille; v++) {
        if (TV[v].index == -1) parcours(v);
    }

    free(TV);
    stack_free(P);

    return partition;
}


void free_partition(t_partition *p) {
    for (int i = 0; i < p->nbClasses; i++) {
        free(p->classes[i].sommets);
    }
    free(p->classes);
    p->classes = NULL;
    p->nbClasses = 0;
}