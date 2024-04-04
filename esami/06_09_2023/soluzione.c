#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATH 4096

typedef struct {
    char input_file[MAX_PATH];
    int n;
    int m;
} parameters;

parameters readInput(int argc, char **argv) {
    // Controllo se il numero di parametri è sufficiente
    if (argc != 4) {
        printf("Usage %s <input-file> <n> <m>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    parameters p;

    strncpy(p.input_file, argv[1], MAX_PATH);
    p.n = atoi(argv[2]);
    p.m = atoi(argv[3]);

    if (p.n < 5 || p.n > 10) {
        printf("Il valore di n è al di fuori del range [5,10]\n");
        exit(EXIT_FAILURE);
    }

    if (p.m < 5 || p.m > 10) {
        printf("Il valore di m è al di fuori del range [5,10]\n");
        exit(EXIT_FAILURE);
    }

    return p;
}

double ***allocateMatrix(int n, int m) {
    double ***matrix = (double ***)malloc(sizeof(double **) * n);
    if (matrix == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        matrix[i] = (double **)malloc(sizeof(double *) * m);
        if (matrix[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < m; j++) {
            matrix[i][j] = (double *)malloc(sizeof(double));
            if (matrix[i][j] == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
        }
    }

    return matrix;
}

void freeMatrix(double ***matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            free(matrix[i][j]);
        }

        free(matrix[i]);
    }

    free(matrix);
}

void fillMatrix(const parameters *p, double ***matrix) {
    FILE *f;

    if ((f = fopen(p->input_file, "r")) == NULL) {
        perror("Errore nell'apertura del file di input");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < p->n; i++) {
        for (int j = 0; j < p->m; j++) {
            if (fscanf(f, "%lf ", matrix[i][j]) == EOF) {
                fprintf(stderr, "Errore nel parsing del file.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(f);
}

void printMatrix(double ***matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%lf\t", *matrix[i][j]);
        }
        printf("\n");
    }
}

void normalize(double ***input, double ***output, int n, int m) {
    double max;

    // Itero sulle colonne della matrice in input
    for (int i = 0; i < m; i++) {
        // Calcolo il massimo per l'i-esima colonna
        max = *input[0][i];

        for (int j = 1; j < n; j++) {
            if (*input[j][i] > max) {
                max = *input[j][i];
            }
        }

        // Imposto i valori della matrice di output normalizzati
        for (int j = 0; j < n; j++) {
            *output[j][i] = *input[j][i] / max;
        }
    }
}

int main(int argc, char **argv) {
    // Punto A
    parameters p = readInput(argc, argv);

    // Punto B
    double ***A = allocateMatrix(p.n, p.m);
    fillMatrix(&p, A);

    // Punto C
    double ***B = allocateMatrix(p.n, p.m);
    normalize(A, B, p.n, p.m);

    // Punto D
    printf("** A **\n");
    printMatrix(A, p.n, p.m);
    printf("** B **\n");
    printMatrix(B, p.n, p.m);

    freeMatrix(A, p.n, p.m);
    freeMatrix(B, p.n, p.m);
}