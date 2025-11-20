//
// Created by mouro on 20/11/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hasse.h"

int link_exists(t_link_array *A, int from, int to) {
    for (int i = 0; i < A->count; i++) {
        if (A->links[i].from == from &&
            A->links[i].to == to)
            return 1;
    }
    return 0;
}

void add_link(t_link_array *A, int from, int to) {
    if (link_exists(A, from, to)) return;

    A->links = realloc(A->links, (A->count + 1) * sizeof(t_link));
    A->links[A->count].from = from;
    A->links[A->count].to = to;
    A->count++;
}

t_link_array build_class_links(liste_adjacence g,
                               t_tarjan_vertex *TV,
                               t_partition P)
{
    t_link_array A;
    A.links = NULL;
    A.count = 0;

    // Tableau : pour chaque sommet, la classe qu'il occupe
    int *classe_sommet = calloc(g.taille, sizeof(int));

    for (int c = 0; c < P.nbClasses; c++) {
        for (int k = 0; k < P.classes[c].nb; k++) {
            int sommet = P.classes[c].sommets[k] - 1;
            classe_sommet[sommet] = c;
        }
    }

    // Parcours des arêtes du graphe pour construire les liens entre classes
    for (int i = 0; i < g.taille; i++) {

        int Ci = classe_sommet[i];

        cellule *cur = g.tab[i].head;
        while (cur) {
            int j = cur->arrivee - 1;
            int Cj = classe_sommet[j];

            if (Ci != Cj) {
                add_link(&A, Ci, Cj);
            }

            cur = cur->suiv;
        }
    }

    free(classe_sommet);
    return A;
}

void export_hasse_to_mermaid(t_partition P, t_link_array A, const char *filename)
{
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("Mermaid export");
        return;
    }

    fprintf(f, "graph TD\n");

    for (int c = 0; c < P.nbClasses; c++) {
        fprintf(f, "    %s[\"%s: {", P.classes[c].name, P.classes[c].name);
        for (int k = 0; k < P.classes[c].nb; k++) {
            fprintf(f, "%d", P.classes[c].sommets[k]);
            if (k < P.classes[c].nb - 1) fprintf(f, ", ");
        }
        fprintf(f, "}\"]\n");
    }
    for (int i = 0; i < A.count; i++) {
        t_link L = A.links[i];
        fprintf(f, "    %s --> %s\n",
                P.classes[L.from].name,
                P.classes[L.to].name);
    }

    fclose(f);
    printf("Diagramme de Hasse exporté dans %s\n", filename);
}

void free_link_array(t_link_array *A) {
    free(A->links);
    A->links = NULL;
    A->count = 0;
}