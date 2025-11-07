//
// Created by Bourrich Noha on 07/11/2025.
//

#ifndef GRAPHE_H
#define GRAPHE_H

typedef struct cellule {
  int arrivee;
  float proba;
  struct cellule *suiv;
} cellule;

typedef struct {
  cellule *head
} liste;

typedef struct {
  liste *tab;
  int taille;
} liste_adjacence;

cellule *creerCellule(int arrivee, float proba);
liste creerliste();
void ajouterCellule(liste *L, int arrivee, float proba);
void afficherListe(liste L);
liste_adjacence creerListeAdjacence(int taille);
void afficherListeAdjacence(liste_adjacence g);

liste_adjacence readGraphe(const char *filename);


void freeListeAdjacence(liste_adjacence *g);

int estGrapheDeMarkov(liste_adjacence g);


char *getId(int nul);
void exportToMermaid(liste_adjacence g, const char *filename);


#endif //GRAPHE_H
