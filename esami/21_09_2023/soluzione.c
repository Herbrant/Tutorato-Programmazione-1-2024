#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 31
#define DEBUG 0

typedef struct {
    char input_file[4096];
    int k;
} params;

params readInput(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input-file> <k>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    params p;

    strncpy(p.input_file, argv[1], 4096);

    p.k = atoi(argv[2]);

    if (p.k < 5 || p.k > 15) {
        fprintf(stderr, "Il numero k: %d non rientra nel range [5,15]\n", p.k);
        exit(EXIT_FAILURE);
    }

    return p;
}

typedef struct node {
    char buffer[BUFFER_SIZE];
    struct node *next;
} node_t;

typedef node_t *stack;

void pushWord(stack *s, char *word) {
    node_t *n = (node_t *)malloc(sizeof(node_t));
    strncpy(n->buffer, word, BUFFER_SIZE);

    n->next = *s;
    *s = n;
}

node_t *popWord(stack *s) {
    node_t *tmp = *s;

    if (tmp != NULL) {
        *s = tmp->next;
    }

    return tmp;
}

stack buildStack(char *input_file) {
    char buffer[BUFFER_SIZE];
    stack s = NULL;

    FILE *f;

    if ((f = fopen(input_file, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (fscanf(f, "%s\n", buffer) != EOF) {
        pushWord(&s, buffer);
    }

    fclose(f);
    return s;
}

char *sortWord(char *word) {
    char *sorted_string = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    strncpy(sorted_string, word, BUFFER_SIZE);

    for (int i = 1; i < strlen(sorted_string); i++) {
        int j = i - 1;
        char tmp = sorted_string[i];

        while (j >= 0 && tmp < sorted_string[j]) {
            sorted_string[j + 1] = sorted_string[j];
            j--;
        }

        sorted_string[j + 1] = tmp;
    }

    return sorted_string;
}

void printStack(stack s, int k) {
    node_t *tmp;
    char *tmp_s;

    while (s != NULL) {
        tmp = popWord(&s);

        if (strlen(tmp->buffer) < k) {
            printf("%s\n", tmp->buffer);
        } else {
            tmp_s = sortWord(tmp->buffer);
            printf("%s\n", tmp_s);
            free(tmp_s);
        }

        free(tmp);
    }
}

int main(int argc, char **argv) {
    params p = readInput(argc, argv);

    if (DEBUG) {
        printf("DEBUG: input_file: %s\n", p.input_file);
        printf("DEBUG: k = %d\n", p.k);
    }

    stack s = buildStack(p.input_file);
    printStack(s, p.k);
}