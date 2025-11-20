//
// Created by mouro on 20/11/2025.
//

#ifndef HASSE_H
#define HASSE_H
#include "tarjan.h"
#include "graphe.h"

typedef struct {
    int from;
    int to;
} t_link;

typedef struct {
    t_link *links;
    int count;
} t_link_array;

t_link_array build_class_links(liste_adjacence g, t_tarjan_vertex *TV, t_partition P);

int link_exists(t_link_array *A, int from, int to);

void add_link(t_link_array *A, int from, int to);

void export_hasse_to_mermaid(t_partition P, t_link_array A, const char *filename);

void free_link_array(t_link_array *A);

#endif
