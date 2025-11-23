#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    const char *filename = (argc >= 2) ? argv[1] : "data/exemple.txt";

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
    for (int i = 0; i < L.count; i++)
        printf("%s --> %s\n",
               P.classes[L.links[i].from].name,
               P.classes[L.links[i].to].name);

    printf("\n=== CARACTERISTIQUES DU GRAPHE ===\n");
    printf(graphe_irreductible(P) ?
           "Le graphe est irreductible.\n" :
           "Le graphe n'est pas irreductible.\n");
    for (int c = 0; c < P.nbClasses; c++) {
        printf("%s : %s\n", P.classes[c].name,
               classe_transitoire(L, c) ? "TRANSITOIRE" : "PERSISTANTE");
        if (classe_absorbante(P, L, c))
            printf(" -> Classe ABSORBANTE (état unique)\n");
    }

    // ==============================
    // Lecture du fichier exemple_meteo.txt
    // ==============================
    const char *meteo_file = "data/exemple_meteo.txt";
    FILE *f = fopen(meteo_file, "r");
    if (!f) { perror("Impossible d'ouvrir le fichier meteo"); exit(1); }

    int from, to; double p; int max_state = 0; char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
            if (from > max_state) max_state = from;
            if (to > max_state) max_state = to;
        }
    }
    int n = max_state;

    double **M = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) M[i] = calloc(n, sizeof(double));
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3)
            M[from-1][to-1] = p;
    }
    fclose(f);

    printf("\nLecture du fichier : %s\n", meteo_file);
    printf("\n=== MATRICE DE TRANSITION M ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%.2f ", M[i][j]);
        printf("\n");
    }

    // ==============================
    // Calcul M^3 et M^7 uniquement pour exemple_meteo.txt
    // ==============================
    double **M2 = multiply_matrices(n, M, M);
    double **M3 = multiply_matrices(n, M2, M);
    printf("\n=== M^3 ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%.2f ", M3[i][j]);
        printf("\n");
    }

    double **M6 = multiply_matrices(n, M3, M3);
    double **M7 = multiply_matrices(n, M6, M);
    printf("\n=== M^7 ===\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%.2f ", M7[i][j]);
        printf("\n");
    }

    // ==============================
    // Convergence M^n pour tous les fichiers exemples
    // ==============================
    const char *all_files[] = {"data/exemple.txt", "data/exemple_meteo.txt",
                               "data/exemple1.txt", "data/exemple2.txt", "data/exemple3.txt"};
    int n_files = 5;
    double epsilon = 0.01;
    int max_iter = 1000;

    printf("\n=== CONVERGENCE M^n ===\n");
    for (int f_idx = 0; f_idx < n_files; f_idx++) {
        const char *file = all_files[f_idx];
        FILE *fp = fopen(file, "r");
        if (!fp) { printf("Impossible d'ouvrir %s\n", file); continue; }

        int max_state_file = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
                if (from > max_state_file) max_state_file = from;
                if (to > max_state_file) max_state_file = to;
            }
        }
        int n_file = max_state_file;

        double **M_file = malloc(n_file * sizeof(double*));
        for (int i = 0; i < n_file; i++) M_file[i] = calloc(n_file, sizeof(double));

        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3)
                M_file[from-1][to-1] = p;
        }
        fclose(fp);

        double **M_prev = create_zero_matrix(n_file);
        copy_matrix(n_file, M_file, M_prev);
        double **M_curr = NULL;
        int converged = 0;

        for (int iter = 1; iter <= max_iter; iter++) {
            M_curr = multiply_matrices(n_file, M_prev, M_file);
            double diff = diff_matrices(n_file, M_curr, M_prev);
            if (diff < epsilon) { converged = 1; break; }
            for (int i = 0; i < n_file; i++) {
                free(M_prev[i]);
                M_prev[i] = M_curr[i];
            }
            free(M_curr);
        }

        if (converged) printf("Convergence atteinte pour %s\n", file);
        else printf("Convergence non atteinte pour %s\n", file);

        for (int i = 0; i < n_file; i++) free(M_file[i]), free(M_prev[i]);
        free(M_file); free(M_prev);
    }

    // ==============================
    // DISTRIBUTIONS STATIONNAIRES
    // ==============================
    const char *meteo_files[] = {"data/exemple.txt", "data/exemple_meteo.txt"};
    int num_files = 2;

    for (int f_idx = 0; f_idx < num_files; f_idx++) {
        const char *file = meteo_files[f_idx];
        FILE *fp = fopen(file, "r");
        if (!fp) { printf("Impossible d'ouvrir %s\n", file); continue; }

        int max_state_file = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3) {
                if (from > max_state_file) max_state_file = from;
                if (to > max_state_file) max_state_file = to;
            }
        }
        int n_file = max_state_file;
        double **M_file = malloc(n_file * sizeof(double*));
        for (int i = 0; i < n_file; i++) M_file[i] = calloc(n_file, sizeof(double));

        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d %d %lf", &from, &to, &p) == 3)
                M_file[from-1][to-1] = p;
        }
        fclose(fp);

        printf("\n=== DISTRIBUTIONS STATIONNAIRES POUR %s ===\n", file);

        for (int c = 0; c < P.nbClasses; c++) {
            if (!classe_persistante(L, c)) {
                printf("Classe %s : transitoire, pas de distribution stationnaire.\n", P.classes[c].name);
                continue;
            }

            t_matrix full_matrix; full_matrix.n = n_file; full_matrix.data = M_file;
            t_matrix sub = subMatrix(full_matrix, P, c);
            int size = sub.n;

            double *pi = malloc(size * sizeof(double));
            double *pi_next = malloc(size * sizeof(double));
            for (int i = 0; i < size; i++) pi[i] = 1.0 / size;

            for (int iter = 0; iter < 1000; iter++) {
                for (int j = 0; j < size; j++) {
                    pi_next[j] = 0.0;
                    for (int k = 0; k < size; k++)
                        pi_next[j] += pi[k] * sub.data[k][j];
                }
                double diff = 0.0;
                for (int i = 0; i < size; i++) diff += fabs(pi_next[i] - pi[i]);
                if (diff < 1e-6) break;
                for (int i = 0; i < size; i++) pi[i] = pi_next[i];
            }

            printf("Classe %s : distribution stationnaire = {", P.classes[c].name);
            for (int i = 0; i < size; i++) {
                printf("%.4f", pi[i]);
                if (i < size-1) printf(", ");
            }
            printf("}\n");

            free(pi); free(pi_next);
            for (int i = 0; i < size; i++) free(sub.data[i]);
            free(sub.data);
        }

        for (int i = 0; i < n_file; i++) free(M_file[i]);
        free(M_file);
    }

    // Libération finale
    for (int i = 0; i < n; i++) free(M[i]);
    free(M);

    freeListeAdjacence(&g);
    free_partition(&P);
    free_link_array(&L);
    free(TV);

    return 0;
}
