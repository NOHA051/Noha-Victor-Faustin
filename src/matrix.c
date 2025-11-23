//
// Created by Kaspereit on 22/11/2025.
//

#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double** create_zero_matrix(int n) {
    double** matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)calloc(n, sizeof(double)); 
    }
    return matrix;
}

void copy_matrix(int n, double** source, double** dest) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dest[i][j] = source[i][j];
        }
    }
}

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

double diff_matrices(int n, double** A, double** B) {
    double diff = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            diff += fabs(A[i][j] - B[i][j]);
        }
    }
    return diff;
}

double** adjacency_to_transition_matrix(int n, int** adjacency_list, int* degrees) {
    double** matrix = create_zero_matrix(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < degrees[i]; j++) {
            int dest = adjacency_list[i][j];
            matrix[i][dest] = 1.0 / degrees[i]; 
        }
    }
    return matrix;
}


t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index) {
    t_classe C = part.classes[compo_index];
    int size = C.nb;

    t_matrix sub;
    sub.n = size;
    sub.data = create_zero_matrix(size);

    for (int i = 0; i < size; i++) {
        int row = C.sommets[i] - 1; 
        for (int j = 0; j < size; j++) {
            int col = C.sommets[j] - 1;
            sub.data[i][j] = matrix.data[row][col];
        }
    }

    return sub;
}
