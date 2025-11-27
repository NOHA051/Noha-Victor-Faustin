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

    printf("\nEst-ce bien un graphe de Markov ?\n\n");
    estGrapheDeMarkov(g);

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

    const char *meteo_file = "data/exemple_meteo.txt";
    FILE *f = fopen(meteo_file, "r");
    if (!f) { perror("Impossible d'ouvrir le fichier meteo"); exit(1); }

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

    int powers[] = {3, 7};
    for (int idx = 0; idx < 2; idx++) {
        int k = powers[idx];
        double **M_power = create_zero_matrix(n);
        copy_matrix(n, M, M_power);

        for (int iter = 1; iter < k; iter++) {
            double **tmp = multiply_matrices(n, M_power, M);
            for (int i = 0; i < n; i++) free(M_power[i]);
            free(M_power);
            M_power = tmp;
        }

        printf("\nM^%d :\n", k);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) printf("%.2f ", M_power[i][j]);
            printf("\n");
        }

        for (int i = 0; i < n; i++) free(M_power[i]);
        free(M_power);
    }


    const char *files_mn[] = {"data/exemple.txt", "data/exemple_meteo.txt",
                              "data/exemple1.txt", "data/exemple2.txt", "data/exemple3.txt"};
    int nb_files_mn = 5;
    double eps = 0.01;
    int max_iter = 1000;

    for (int f_idx = 0; f_idx < nb_files_mn; f_idx++) {
        const char *file = files_mn[f_idx];
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
        double **M_curr = create_zero_matrix(n_file);
        copy_matrix(n_file, M_file, M_curr);

        int converged = 0;
        int n_power = 1;
        for (int iter = 0; iter < max_iter; iter++) {
            copy_matrix(n_file, M_curr, M_prev);
            double **M_next = multiply_matrices(n_file, M_curr, M_file);

            for (int i = 0; i < n_file; i++) free(M_curr[i]);
            free(M_curr);
            M_curr = M_next;

            double diff = diff_matrices(n_file, M_curr, M_prev);
            n_power++;

            if (diff < eps) {
                converged = 1;
                break;
            }
        }

        if (converged) {
            printf("\n=== Convergence atteinte pour %s apres n = %d ===\n", file, n_power);
            printf("M^%d :\n", n_power);
            for (int i = 0; i < n_file; i++) {
                for (int j = 0; j < n_file; j++) printf("%.2f ", M_curr[i][j]);
                printf("\n");
            }
        } else {
            printf("\nConvergence non atteinte pour %s avec eps = %.2f apres %d iterations \n",
                   file, eps, max_iter);
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

    const char *meteo_files[] = {"data/exemple.txt", "data/exemple_meteo.txt",
                                 "data/exemple1.txt", "data/exemple2.txt", "data/exemple3.txt"};
    int num_files = 5;

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

        printf("\n=== DISTRIBUTIONS STATIONNAIRES POUR %s ===\n", file);

        for (int c = 0; c < P.nbClasses; c++) {
            if (!classe_persistante(L, c)) {
                printf("Classe %s : transitoire, pas de distribution stationnaire.\n", P.classes[c].name);
                continue;
            }

            t_matrix full_matrix;
            full_matrix.n = n_file;
            full_matrix.data = M_file;
            t_matrix sub = subMatrix(full_matrix, P, c);
            int size = sub.n;

            double epsilon = 1e-6;
            int max_iter_station = 1000;
            double *pi = malloc(size * sizeof(double));
            double *pi_next = malloc(size * sizeof(double));
            for (int i = 0; i < size; i++) pi[i] = 1.0 / size;

            for (int iter = 0; iter < max_iter_station; iter++) {
                for (int j = 0; j < size; j++) {
                    pi_next[j] = 0.0;
                    for (int k = 0; k < size; k++)
                        pi_next[j] += pi[k] * sub.data[k][j];
                }
                double diff = 0.0;
                for (int i = 0; i < size; i++) diff += fabs(pi_next[i] - pi[i]);
                if (diff < epsilon) break;
                for (int i = 0; i < size; i++) pi[i] = pi_next[i];
            }

            printf("Classe %s : distribution stationnaire = {", P.classes[c].name);
            for (int i = 0; i < size; i++) {
                printf("%.4f", pi[i]);
                if (i < size - 1) printf(", ");
            }
            printf("}\n");

            free(pi);
            free(pi_next);
            for (int i = 0; i < size; i++) free(sub.data[i]);
            free(sub.data);
        }

        for (int i = 0; i < n_file; i++) free(M_file[i]);
        free(M_file);
    }

// **** Création du fichier Mermaid pour l'étape 3 de la partie 1 ****//

    g = readGraphe("/Users/bourrichnoha/CLionProjects/Noha-Victor-Faustin/data/exempleM.txt");

    writeMermaid(g, "/Users/bourrichnoha/CLionProjects/Noha-Victor-Faustin/src/grapheMermaid.mmd");   // ← Nom choisi par toi !

    printf("Fichier Mermaid généré.\n");



    for (int i = 0; i < n; i++) free(M[i]);
    free(M);

    freeListeAdjacence(&g);
    free_partition(&P);
    free_link_array(&L);
    free(TV);

    return 0;
}
