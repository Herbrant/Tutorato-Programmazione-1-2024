// Implementazione di una lista singolarmente linkata
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// definisco un tipo per rappresentare il nodo della lista
typedef struct node {
    int value;
    struct node *next;
} node_t;

// definisco il tipo list come un puntatore a node_t (il puntatore al primo
// elemento della lista)
typedef node_t *list;

// insert inserisce un elemento in testa alla lista l
// NOTA: passiamo un puntatore a list (node_t **) in quanto abbiamo la necessità
// di modificare il puntatore a node_t (la testa della lista)
void list_insert(list *l, int val) {
    // alloco sull'heap un'area di memoria di dimensione pari a quella
    // necessaria per contenere un node_t
    node_t *n = malloc(sizeof(node_t));
    n->value = val; // imposto il campo value del nuovo nodo a val
    n->next = *l;   // il successore del nuovo nodo è la vecchia testa
    *l = n;         // la nuova testa è il nuovo nodo (n)
}

// funzione di stampa per la lista
void list_print(list l) {
    // itero fin quando il puntatore l è diverso da NULL (la lista non è finita)
    // NOTA: posso modificare il valore di l in quanto questa è una copia del
    // puntatore alla testa
    while (l != NULL) {
        printf("%d ", l->value);
        l = l->next;
    }

    printf("\n");
}

// search effettua una ricerca sulla lista per valore
// restituisce un puntatore al nodo contenente il valore se questo esiste, NULL
// altrimenti
node_t *list_search(list l, int val) {
    // itero fin quando il puntatore l è diverso da NULL (la lista non è finita)
    // NOTA: posso modificare il valore di l in quanto questa è una copia del
    // puntatore alla testa
    while (l != NULL) {
        // se il valore contenuto nel nodo è quello che sto cercando restutisco
        // il puntatore al nodo stesso
        if (l->value == val) {
            return l;
        }

        l = l->next;
    }

    // se non ho trovato un nodo con campo value pari a val restituisco NULL
    return NULL;
}

// exists restituisce true se il valore passato come parametro è presente in
// almeno un nodo della lista, false altrimenti
bool list_exists(list l, int val) { return list_search(l, val) != NULL; }

bool list_remove(list *l, int val) {
    node_t *ptr = *l;
    node_t *tmp;

    // Caso 1: l'elemento che voglio cancellare è in testa
    // Verifico che la lista non è vuota (ergo la testa è diversa da NULL) e se
    // il valore della testa è uguale a val
    // NOTA: in questo caso l'ordine delle condizioni è importante
    if (ptr != NULL && ptr->value == val) {
        *l = ptr->next;
        free(ptr);
        return true;
    }

    // Caso 2: l'elemento che voglio cancellare non si trova in testa
    // itero finché la lista non è finita e finché non ho trovato un nodo con
    // campo value pari a val NOTA: anche qui l'ordine delle condizioni è
    // importante
    while (ptr != NULL && ptr->value != val) {
        tmp = ptr;
        ptr = ptr->next;
    }

    // verifico se ptr è diverso da NULL (ergo ho trovato il nodo durante il
    // ciclo precedente)
    if (ptr != NULL) {
        tmp->next = ptr->next;
        free(ptr);
        return true;
    }

    // restituisco false nel caso in cui non ho trovato il nodo da eliminare
    return false;
}

// list_destroy distrugge la lista
// NOTA: tale funzione è necessaria in quanto tutti i nodi sono allocati
// nell'heap (dinamicamente)
void list_destroy(list l) {
    node_t *ptr;

    while (l != NULL) {
        ptr = l;
        l = l->next;
        free(ptr);
    }
}

int main() {
    list l = NULL;
    list_insert(&l, 10);
    list_insert(&l, 20);
    list_insert(&l, 300);

    // Stampo la lista
    list_print(l);

    // Ricerca
    if (list_exists(l, 10)) {
        printf("Il numero 10 è presente in lista\n");
    } else {
        printf("Il numero 10 non è presente in lista\n");
    }

    if (list_exists(l, 100)) {
        printf("Il numero 100 è presente in lista\n");
    } else {
        printf("Il numero 100 non è presente in lista\n");
    }

    // Cancello il nodo 300
    list_remove(&l, 300);
    // Cancello il nodo 10
    list_remove(&l, 10);

    // Stampo la lista
    list_print(l);

    // Distruggo la lista
    list_destroy(l);
}