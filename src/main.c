#include <stdio.h>
#include <stdlib.h>

#include "graphe.h"
#include "tarjan.h"
#include "hasse.h"

int classe_transitoire(t_link_array links, int classe_id) {
    for (int i = 0; i < links.count; i++) {
        if (links.links[i].from == classe_id)
            return 1;
    }
    return 0;
}

int classe_persistante(t_link_array links, int classe_id) {
    return !classe_transitoire(links, classe_id);
}

int classe_absorbante(t_partition P, t_link_array links, int classe_id) {
    return (P.classes[classe_id].nb == 1 &&
            classe_persistante(links, classe_id));
}

int graphe_irreductible(t_partition P) {
    return (P.nbClasses == 1);
}


int main(int argc, char *argv[]) {

    const char *filename = (argc >= 2)
                           ? argv[1]
                           : "data/exemple.txt";

    printf("Lecture du fichier : %s\n", filename);
    liste_adjacence g = readGraphe(filename);

    printf("Graphe charge. Nombre de sommets = %d\n\n", g.taille);

    printf("=== LISTE D'ADJACENCE ===\n");
    afficherListeAdjacence(g);

    printf("\n=== TARJAN : PARTITION EN CLASSES ===\n");
    t_partition P = tarjan(g);

    for (int c = 0; c < P.nbClasses; c++) {
        printf("%s: {", P.classes[c].name);
        for (int k = 0; k < P.classes[c].nb; k++) {
            printf("%d", P.classes[c].sommets[k]);
            if (k < P.classes[c].nb - 1) printf(", ");
        }
        printf("}\n");
    }

    printf("\n=== DIAGRAMME DE HASSE ===\n");

    t_tarjan_vertex *TV = init_tarjan_vertices(g);

    t_link_array L = build_class_links(g, TV, P);

    for (int i = 0; i < L.count; i++) {
        printf("%s --> %s\n",
               P.classes[L.links[i].from].name,
               P.classes[L.links[i].to].name);
    }


    printf("\n=== CARACTERISTIQUES DU GRAPHE ===\n");

    if (graphe_irreductible(P))
        printf("Le graphe est irreductible.\n");
    else
        printf("Le graphe n'est pas irreductible.\n");

    printf("\nEtats et classes :\n");

    for (int c = 0; c < P.nbClasses; c++) {
        if (classe_transitoire(L, c))
            printf("%s : classe TRANSITOIRE\n", P.classes[c].name);
        else
            printf("%s : classe PERSISTANTE\n", P.classes[c].name);

        if (classe_absorbante(P, L, c))
            printf(" -> Classe ABSORBANTE (état unique)\n");
    }

    freeListeAdjacence(&g);
    free_partition(&P);
    free_link_array(&L);
    free(TV);

    return 0;
}