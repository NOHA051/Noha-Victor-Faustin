#include <stdio.h>

#include "graphe.h"
#include "graphe.c"

int main(int argc, char *argv[]) {
    const char *filename;
    if (argc >= 2) filename = argv[1];
    else filename = "../data/exemple1.txt"; // chemin par défaut si aucun argument

    printf("Lecture du fichier : %s\n", filename);
    liste_adjacence g = readGraph(filename);
    printf("Graphe chargé. Nombre de sommets = %d\n\n", g.taille);

    // Étape 1 :  affichage du graphe
    afficherListeAdjacence(g);

    // Étape 2 : vérification Markov
    printf("\n--- Vérification du graphe de Markov ---\n");
    int ok = estGrapheDeMarkov(g);

    if (ok)
        printf("\n Le graphe est bien un graphe de Markov.\n");
    else
        printf("\n Le graphe n'est pas un graphe de Markov.\n");

    // Étape 3 : export vers Mermaid
    printf("\n--- Exportation du graphe au format Mermaid ---\n");
    exportToMermaid(g, "exemple_valid_step3.txt");

    // Libération de la mémoire
    freeListeAdjacence(&g);

    return 0;
}
