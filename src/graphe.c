#include "graphe.h"

#include <stdio.h>
#include <stdlib.h>


cellule *creerCellule(int arrivee, float proba) {
    cellule *c = (cellule *)malloc(sizeof(cellule));
    if (!c) { perror("malloc"); exit(EXIT_FAILURE); }
    c->arrivee = arrivee;
    c->proba = proba;
    c->suiv = NULL;
    return c;
}

liste creerListe() {
    liste L;
    L.head = NULL;
    return L;
}

void ajouterCellule(liste *L, int arrivee, float proba) {
    cellule *c = creerCellule(arrivee, proba);
    c->suiv = L->head;
    L->head = c;
}

void afficherListe(liste L) {
    cellule *cur = L.head;
    while (cur != NULL) {
        printf(" -> (%d, %.2f)", cur->arrivee, cur->proba);
        cur = cur->suiv;
    }
    printf("\n");
}

liste_adjacence creerListeAdjacence(int taille) {
    liste_adjacence g;
    g.taille = taille;
    g.tab = (liste *)malloc(taille * sizeof(liste));
    if (!g.tab) { perror("malloc"); exit(EXIT_FAILURE); }
    for (int i = 0; i < taille; ++i) g.tab[i] = creerListe();
    return g;
}

void afficherListeAdjacence(liste_adjacence g) {
    for (int i = 0; i < g.taille; ++i) {
        printf("Sommet %d :", i + 1);
        afficherListe(g.tab[i]);
    }
}

/* ---------- Lecture du fichier (readGraph) ---------- */

liste_adjacence readGraphe(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (file == NULL) { perror(filename); exit(EXIT_FAILURE); }

    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        fprintf(stderr, "Could not read number of vertices from %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    liste_adjacence g = creerListeAdjacence(nbvert);

    int depart, arrivee;
    float proba;
    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        if (depart < 1 || depart > nbvert || arrivee < 1 || arrivee > nbvert) {
            fprintf(stderr, "Warning: invalid edge %d -> %d (ignored)\n", depart, arrivee);
            continue;
        }
        ajouterCellule(&g.tab[depart - 1], arrivee, proba);
    }

    fclose(file);
    return g;
}

/* ---------- Libération de la mémoire ---------- */

void freeListeAdjacence(liste_adjacence *g) {
    for (int i = 0; i < g->taille; ++i) {
        cellule *cur = g->tab[i].head;
        while (cur) {
            cellule *tmp = cur;
            cur = cur->suiv;
            free(tmp);
        }
        g->tab[i].head = NULL;
    }
    free(g->tab);
    g->tab = NULL;
    g->taille = 0;
}



int estGrapheDeMarkov(liste_adjacence g) {
    int estValide = 1;

    for (int i = 0; i < g.taille; i++) {
        cellule *cur = g.tab[i].head;
        float somme = 0.0f;

        while (cur != NULL) {
            somme += cur->proba;
            cur = cur->suiv;
        }

        if (somme < 0.99f || somme > 1.01f) {
            printf("Sommet %d : somme des probabilites = %.3f \n", i + 1, somme);
            estValide = 0;
        } else {
            printf("Sommet %d : somme des probabilites = %.3f \n", i + 1, somme);
        }

    }
    if (estValide == 0) {
        printf("\nCe n'est pas un graphe de Markov\n");
    } else {
        printf("\nC'est bien un graphe de Markov\n");
    }

    return estValide;
}



char *getId(int num) {
    static char buffer[10];
    int i = 0;
    num--;

    while (num >= 0) {
        buffer[i++] = 'A' + (num % 26);
        num = num / 26 - 1;
    }
    buffer[i] = '\0';

    // reverse
    for (int j = 0; j < i/2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i-j-1];
        buffer[i-j-1] = tmp;
    }

    return buffer;
}


void writeMermaid(liste_adjacence g, const char *filename) {
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("Erreur ouverture fichier Mermaid");
        exit(EXIT_FAILURE);
    }

    // --- CONFIGURATION MERMAID ---
    fprintf(f,
        "---\n"
        "config:\n"
        "   layout: elk\n"
        "   theme: neo\n"
        "   look: neo\n"
        "---\n\n"
        "flowchart LR\n"
    );

    // --- DÉCLARATION DES SOMMETS ---
    for (int i = 0; i < g.taille; i++) {
        char *id = getId(i + 1);
        fprintf(f, "%s((%d))\n", id, i + 1);
    }
    fprintf(f, "\n");

    // --- ARÊTES ---
    for (int i = 0; i < g.taille; i++) {
        cellule *cur = g.tab[i].head;
        char *idDepart = getId(i + 1);

        while (cur != NULL) {
            char *idArrivee = getId(cur->arrivee);
            fprintf(f, "%s -->|%.2f| %s\n", idDepart, cur->proba, idArrivee);
            cur = cur->suiv;
        }
    }

    fclose(f);
}
