//
// Created by Kaspereit on 22/11/2025.
//

#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <math.h>

typedef struct {
    int n;          
    double **data;  
} t_matrix;

double** create_zero_matrix(int n);

void copy_matrix(int n, double** source, double** dest);

double** multiply_matrices(int n, double** A, double** B);

double diff_matrices(int n, double** A, double** B);

double** adjacency_to_transition_matrix(int n, int** adjacency_list, int* degrees);

#include "tarjan.h"
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index);

#endif
