#include <stdio.h>
#include <stdlib.h>
#include "graphe.h"
#include "tarjan.h"
#include "hasse.h"
#include "matrix.h"

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


    printf("\nLecture du fichier : %s\n", filename);
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

 
    const char *meteo_file = "data/exemple_meteo.txt";
    FILE *f = fopen(meteo_file, "r");
    if (!f) {
        perror("Impossible d'ouvrir le fichier meteo");
        exit(1);
    }

    int from, to;
    double p;
    int max_state = 0;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
            if (from > max_state) max_state = from;
            if (to > max_state) max_state = to;
        }
    }

    int n = max_state;

    double **M = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++)
        M[i] = calloc(n, sizeof(double));

    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
            M[from-1][to-1] = p;
        }
    }
    fclose(f);

    printf("\nLecture du fichier : %s\n", meteo_file);
    printf("\n=== MATRICE DE TRANSITION M ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%.2f ", M[i][j]);
        printf("\n");
    }

    double **M2 = multiply_matrices(n, M, M);
    double **M3 = multiply_matrices(n, M2, M);
    double **M6 = multiply_matrices(n, M3, M3);
    double **M7 = multiply_matrices(n, M6, M);

    printf("\n=== MATRICE M^3 ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%.2f ", M3[i][j]);
        printf("\n");
    }

    printf("\n=== MATRICE M^7 ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%.2f ", M7[i][j]);
        printf("\n");
    }

    const char *meteo_files[] = {
        "data/exemple.txt",
        "data/exemple_meteo.txt"
    };
    int num_files = 2;
    double epsilon = 0.01;
    int max_iter = 1000;

    for (int f_idx = 0; f_idx < num_files; f_idx++) {
        const char *file = meteo_files[f_idx];
        FILE *fp = fopen(file, "r");
        if (!fp) {
            printf("Impossible d'ouvrir %s\n", file);
            continue;
        }

        int max_state_file = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
                if (from > max_state_file) max_state_file = from;
                if (to > max_state_file) max_state_file = to;
            }
        }
        int n_file = max_state_file;

        double **M_file = malloc(n_file * sizeof(double*));
        for (int i = 0; i < n_file; i++)
            M_file[i] = calloc(n_file, sizeof(double));

        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3)
                M_file[from-1][to-1] = p;
        }
        fclose(fp);

        printf("\n==== Test de convergence pour %s ====\n", file);

        double **M_prev = create_zero_matrix(n_file);
        double **M_curr = create_zero_matrix(n_file);
        copy_matrix(n_file, M_file, M_curr);

        int n_power = 1;
        double diff = 0.0;
        int converged = 0;

        for (int iter = 0; iter < max_iter; iter++) {
            copy_matrix(n_file, M_curr, M_prev);
            double **M_next = multiply_matrices(n_file, M_curr, M_file);
            for (int i = 0; i < n_file; i++) free(M_curr[i]);
            free(M_curr);
            M_curr = M_next;

            diff = diff_matrices(n_file, M_curr, M_prev);
            n_power++;

            if (diff < epsilon) {
                converged = 1;
                break;
            }
        }

        if (converged) {
            printf("Convergence atteinte pour n = %d (diff = %.6f)\n", n_power, diff);
            printf("=== MATRICE LIMITE M^%d ===\n", n_power);
            for (int i = 0; i < n_file; i++) {
                for (int j = 0; j < n_file; j++)
                    printf("%.2f ", M_curr[i][j]);
                printf("\n");
            }
        } else {
            printf("La matrice ne converge pas avec epsilon = %.2f (après %d iterations)\n", epsilon, max_iter);
        }

        for (int i = 0; i < n_file; i++) {
            free(M_file[i]);
            free(M_prev[i]);
            free(M_curr[i]);
        }
        free(M_file);
        free(M_prev);
        free(M_curr);
    }

    for (int i = 0; i < n; i++) {
        free(M[i]);
        free(M2[i]);
        free(M3[i]);
        free(M6[i]);
        free(M7[i]);
    }
    free(M);
    free(M2);
    free(M3);
    free(M6);
    free(M7);

    return 0;
}
