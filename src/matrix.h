#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

// Création d'une matrice n x n remplie de 0
double** create_zero_matrix(int n);

// Recopie d'une matrice dans une autre
void copy_matrix(int n, double** source, double** dest);

// Multiplication de deux matrices n x n
double** multiply_matrices(int n, double** A, double** B);

// Calcul de la différence entre deux matrices
double diff_matrices(int n, double** A, double** B);

// Création d'une matrice de transition à partir d'une liste d'adjacence
double** adjacency_to_transition_matrix(int n, int** adjacency_list, int* degrees);

#endif
