#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char pathfile[4096];
    float f;
} params;

typedef struct node {
    float v;
    char p[31];
    struct node *next;
    struct node *prev;
} node_t;

typedef node_t *list;

params readInput(int argc, char **argv) {
    params p;

    if (argc != 3) {
        fprintf(stderr, "%s <input-file> <f>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strncpy(p.pathfile, argv[1], 4096);
    p.f = atof(argv[2]);

    if (p.f < 1 || p.f > 20) {
        fprintf(stderr, "Il valore %f non è nel range [1,20]\n", p.f);
        exit(EXIT_FAILURE);
    }

    return p;
}

void insertRecord(list *l, char *p, float f) {
    node_t *n = (node_t *)malloc(sizeof(node_t));

    strncpy(n->p, p, 31);
    n->v = f;

    // Caso 1: la lista è vuota, inserisco l'elemento in testa
    if (*l == NULL) {
        *l = n;
        n->next = n->prev = NULL;
    } else if (strcmp((*l)->p, n->p) > 0) { // Caso 2: il nodo deve prendere il
                                            // posto della testa attuale
        n->prev = NULL;
        n->next = *l;
        (*l)->prev = n;
        *l = n;
    } else { // Caso 3: devo cercare dove inserire il nuovo nodo
        node_t *ptr = (*l)->next;
        node_t *tmp = *l;

        while (ptr != NULL && strcmp(ptr->p, n->p) < 0) {
            tmp = ptr;
            ptr = ptr->next;
        }

        n->next = ptr;
        n->prev = tmp;
        tmp->next = n;

        if (ptr != NULL) {
            ptr->prev = n;
        }
    }
}

void build_list(list *l, char *input_file) {
    FILE *f;

    if ((f = fopen(input_file, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    float value;
    char buffer[31];

    while (fscanf(f, "%f\n%s", &value, buffer) != EOF) {
        insertRecord(l, buffer, value);
    }

    fclose(f);
}

void printList(list l) {
    while (l != NULL) {
        printf("%s %f\n", l->p, l->v);
        l = l->next;
    }
}

unsigned elab(list l, float f) {
    unsigned count = 0;

    while (l != NULL) {
        if (l->v >= f) {
            count++;
        }

        l = l->next;
    }

    return count;
}

// Funzione iterativa per la deallocazione della lista
void list_destroy(list l) {
    node_t *tmp;

    while (l != NULL) {
        tmp = l;
        l = l->next;
        free(tmp);
    }
}

// Funzione ricorsiva per la deallocazione della lista
void list_destroy_rec(list l) {
    if (l == NULL) {
        return;
    }

    list_destroy_rec(l->next);
    free(l);
    return;
}

int main(int argc, char **argv) {
    params p = readInput(argc, argv);
    list l = NULL;

    build_list(&l, p.pathfile);
    unsigned elab_out = elab(l, p.f);
    printf("Numero di record aventi lunghezza pari o maggiore di %f: %u\n", p.f,
           elab_out);
    printList(l);

    // list_destroy(l);
    list_destroy_rec(l);
}