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









#endif //GRAPHE_H
