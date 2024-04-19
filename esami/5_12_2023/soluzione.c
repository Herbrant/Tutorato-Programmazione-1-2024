#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 31

// Struttura per i parametri passati dall'utente
typedef struct {
    char input_file[4096];
} parameters;

parameters readFile(int argc, char **argv) {
    // Verifico che il numero di parametri è sufficiente all'esecuzione del
    // programma
    if (argc != 2) {
        printf("Usage: %s <input-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Costruisco la struttura dei parametri per il programma
    parameters p;
    strncpy(p.input_file, argv[1], 4096);

    return p;
}

// Struttura per il nodo della coda
typedef struct node {
    char s[BUFFER_SIZE];
    struct node *next;
} node_t;

// Struttura per la coda
typedef struct {
    node_t *head;
    node_t *tail;
    unsigned n;
} queue;

// Funzione di inizializzazione per la coda: imposta la testa e la coda a NULL
void init_tail(queue *q) {
    q->head = q->tail = NULL;
    q->n = 0;
}

// Funzione di inserimento nella coda
void enqueueWord(queue *q, char *s) {
    node_t *n = malloc(sizeof(node_t));
    strncpy(n->s, s, BUFFER_SIZE);
    n->next = NULL;

    // Se la coda è vuota allora head e tail puntano al nuovo nodo
    if (q->head == NULL) {
        q->head = q->tail = n;
    } else {               // altrimenti imposto solo tail al nuovo nodo
        q->tail->next = n; // imposto il puntatore a next del penultimo elemento
                           // alla nuova tail
        q->tail = n;
    }

    q->n++;
}

// Funzione di estrazione, carica la stringa presente nel nodo head in word e
// restituisce 1 se l'operaizone è andata a buon fine, 0 altrimenti
int dequeueWord(queue *q, char *word) {
    if (q->head == NULL) {
        return 0;
    }

    strncpy(word, q->head->s, BUFFER_SIZE);
    node_t *tmp = q->head;
    q->head = tmp->next;
    free(tmp);
    q->n--;
    return 1;
}

void buildqueue(queue *q, char *input_file) {
    FILE *f;
    char buffer[BUFFER_SIZE];

    if ((f = fopen(input_file, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, BUFFER_SIZE, f)) {
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        enqueueWord(q, buffer);
    }

    fclose(f);
}

unsigned buildStringArray(queue *q, char **vec) {
    unsigned n = q->n;

    for (int i = 0; i < n; i++) {
        vec[i] = (char *)malloc(sizeof(char) * BUFFER_SIZE);
        dequeueWord(q, vec[i]);
    }

    return n;
}

void printArray(char **vec, int n) {
    for (int i = 0; i < n; i++) {
        printf("%s\n", vec[i]);
    }
}

void sortArray(char **vec, int n) {
    char *tmp;

    for (int i = 1; i < n; i++) {
        int j = i - 1;
        tmp = vec[i];

        while (j >= 0 && strcmp(tmp, vec[j]) < 0) {
            vec[j + 1] = vec[j];
            j--;
        }

        vec[j+1] = tmp;
    }
}

void clearArray(char **vec, int n) {
    for (int i = 0; i < n; i++) {
        free(vec[i]);
    }
}

int main(int argc, char **argv) {
    parameters p = readFile(argc, argv);
    queue q;
    init_tail(&q);

    buildqueue(&q, p.input_file);

    char *vec[q.n];
    unsigned n = buildStringArray(&q, vec);

    sortArray(vec, n);
    printArray(vec, n);
    clearArray(vec, n);
}