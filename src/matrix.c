#include "matrix.h"
#include <stdio.h>
#include <math.h>

// Création d'une matrice n x n remplie de 0
double** create_zero_matrix(int n) {
    double** matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)calloc(n, sizeof(double)); // remplit de 0
    }
    return matrix;
}

// Recopie d'une matrice
void copy_matrix(int n, double** source, double** dest) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dest[i][j] = source[i][j];
        }
    }
}

// Multiplication de matrices
double** multiply_matrices(int n, double** A, double** B) {
    double** result = create_zero_matrix(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

// Différence entre deux matrices (somme des valeurs absolues des différences)
double diff_matrices(int n, double** A, double** B) {
    double diff = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            diff += fabs(A[i][j] - B[i][j]);
        }
    }
    return diff;
}

// Création d'une matrice de transition à partir d'une liste d'adjacence
double** adjacency_to_transition_matrix(int n, int** adjacency_list, int* degrees) {
    double** matrix = create_zero_matrix(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < degrees[i]; j++) {
            int dest = adjacency_list[i][j];
            matrix[i][dest] = 1.0 / degrees[i]; // probabilité de transition uniforme
        }
    }
    return matrix;
}
