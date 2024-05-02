#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char input_file[255];
    char output_file[255];
} params;

typedef struct {
    int n;
    int m;
} matrix_size;

params readInput(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input-file> <output-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    params p;

    strncpy(p.input_file, argv[1], 255);
    strncpy(p.output_file, argv[2], 255);

    return p;
}

matrix_size getMatrixSize(char *input_file) {
    FILE *f;
    char buffer[1024];
    matrix_size m_size;

    m_size.m = m_size.n = 0;

    if ((f = fopen(input_file, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, 1024, f)) {
        m_size.n++;
    }

    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == ' ') {
            m_size.m++;
        }
    }

    m_size.m++;

    fclose(f);
    return m_size;
}

double ***createMatrix(char *input_file, int n, int m) {
    FILE *f;
    if ((f = fopen(input_file, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double ***matrix = (double ***)malloc(sizeof(double **) * n);

    for (int i = 0; i < n; i++) {
        matrix[i] = (double **)malloc(sizeof(double *) * m);

        for (int j = 0; j < m; j++) {
            matrix[i][j] = malloc(sizeof(double));

            if (fscanf(f, "%lf", matrix[i][j]) == EOF) {
                fprintf(stderr, "Errore nel parsing del file\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(f);
    return matrix;
}

void printMatrix(double ***matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j]) {
                printf("%lf ", *matrix[i][j]);
            } else {
                printf("* ");
            }
        }

        printf("\n");
    }
}

void freeMatrix(double ***matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j]) {
                free(matrix[i][j]);
            }
        }

        free(matrix[i]);
    }

    free(matrix);
}

void sparsify(double ***matrix, int n, int m) {
    double mean;

    for (int i = 0; i < n; i++) {
        mean = 0;

        for (int j = 0; j < m; j++) {
            if (matrix[i][j]) {
                mean += *matrix[i][j];
            }
        }

        mean = mean / m;

        for (int j = 0; j < m; j++) {
            if (matrix[i][j] && *matrix[i][j] < mean) {
                free(matrix[i][j]);
                matrix[i][j] = NULL;
            }
        }
    }
}

int collect(double ***matrix, int n, int m, double **arr) {
    int counter = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j]) {
                counter++;
            }
        }
    }

    *arr = (double *)malloc(sizeof(double) * counter);
    int k = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j]) {
                (*arr)[k] = *matrix[i][j];
                k++;
            }
        }
    }

    return counter;
}

void writeToFile(double *arr, int n, char *output_file) {
    FILE *f;

    if ((f = fopen(output_file, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        fprintf(f, "%lf ", arr[i]);
    }

    fprintf(f, "\n");
    fclose(f);
}

int main(int argc, char **argv) {
    params p = readInput(argc, argv);
    matrix_size m_size = getMatrixSize(p.input_file);
    double ***matrix = createMatrix(p.input_file, m_size.n, m_size.m);
    // printMatrix(matrix, m_size.n, m_size.m);
    sparsify(matrix, m_size.n, m_size.m);

    double *arr = NULL;
    int arr_size = collect(matrix, m_size.n, m_size.m, &arr);

    writeToFile(arr, arr_size, p.output_file);

    freeMatrix(matrix, m_size.n, m_size.m);
    free(arr);
}