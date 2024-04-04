#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char input_file[PATH_MAX];
    char output_file[PATH_MAX];
} cmd_input_t;

// Struttura per memorizzare le informazioni del cliente
typedef struct {
    char nome[255];
    char cognome[255];
    int numero_conto;
    int anno_apertura;
    float saldo;
} cliente_t;

// Funzione generica per la stampa di un cliente
// NOTA: passando un FILE* è possibile gestire la stampa su file generici
// (precedentemente aperti) e sullo standard output
void fprintCliente(FILE *f, const cliente_t *c) {
    fprintf(f, "%s %s %d %d %.2f\n", c->nome, c->cognome, c->numero_conto,
            c->anno_apertura, c->saldo);
}

// Struttura del nodo per la lista singolarmente linkata
typedef struct nodo {
    // dati del cliente
    cliente_t *cliente;
    // puntatore al nodo successivo della lista
    struct nodo *next;
} nodo_t;

// Definisco il tipo "lista" come un puntatore a nodo_t (il puntatore alla testa
// della lista)
typedef nodo_t *lista;

void insertList(lista *l, cliente_t *cliente) {
    // Alloco un nuovo nodo sull'heap
    nodo_t *nodo = (nodo_t *)malloc(sizeof(nodo_t));
    // Assegno al campo cliente del nuovo il puntatore al cliente passato come
    // parametro della funzione
    nodo->cliente = cliente;

    // Verifico se la lista è vuota, se lo è inserisco il nuovo nodo in testa
    if (*l == NULL) {
        nodo->next = NULL;
        *l = nodo;

        // Verifico se il nuovo nodo dev'essere
        // inserito al posto dell'attuale testa
    } else if ((*l)->cliente->anno_apertura > nodo->cliente->anno_apertura) {
        nodo->next = *l;
        *l = nodo;

        // Altrimenti, cerco la posizione in cui inserire il nuovo nodo
    } else {
        nodo_t *ptr = *l;

        while (ptr->next != NULL && ptr->next->cliente->anno_apertura <
                                        nodo->cliente->anno_apertura) {
            ptr = ptr->next;
        }

        nodo->next = ptr->next;
        ptr->next = nodo;
    }
}

void removeNode(lista *l, nodo_t *n) {
    // Verifico se l'elemento da eliminare si trova in testa
    if (n == *l) {
        *l = n->next;
        free(n->cliente);
        free(n);
        return;
    }

    // Se l'elemento non si trova in testa lo cerco mediante il suo indirizzo di
    // memoria
    nodo_t *ptr = *l;

    while (ptr->next != NULL && ptr->next != n) {
        ptr = ptr->next;
    }

    if (ptr->next != NULL) {
        ptr->next = n->next;
        free(n->cliente);
        free(n);
    }
}

// Stampa la lista sullo standard output (funzione utile per testare il codice)
void printLista(lista l) {
    nodo_t *ptr = l;

    while (ptr != NULL) {
        fprintCliente(stdout, ptr->cliente);
        ptr = ptr->next;
    }
}

// Funzione che restituisce in output il numero di elemento presenti in lista
// NOTA: chiaramente, una soluzione più efficiente può essere quella di
// utilizzare un contatore all'interno della struttura lista.
unsigned getN(const lista l) {
    nodo_t *ptr = l;
    unsigned counter = 0;

    while (ptr != NULL) {
        counter++;
        ptr = ptr->next;
    }

    return counter;
}

// Funzione per deallocare la lista
void destroyList(lista l) {
    nodo_t *ptr = l;
    nodo_t *tmp;

    // scorro la lista
    while (ptr != NULL) {
        tmp = ptr;
        ptr = ptr->next;
        // rimuovo i dati del cliente
        free(tmp->cliente);
        // rimuovo il nodo
        free(tmp);
    }
}

cmd_input_t *readInput(int argc, char **argv) {
    unsigned len;

    // Verifico che il numero di parametri passati al programma sia sufficiente
    if (argc < 3) {
        fprintf(stderr, "Usage %s <input-file.txt> <output-file.txt>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    // Ripulisco il carattere di terminazione del primo parametro
    len = strlen(argv[1]);

    if (argv[1][len - 1] == '\n') {
        argv[1][len - 1] = '\0';
    }

    // Verifico che il primo parametro contenga l'estensione ".txt"
    if (strcmp(&argv[1][len - 4], ".txt")) {
        fprintf(stderr, "Error: output file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    len = strlen(argv[2]);

    // Ripulisco il carattere di terminazione del secondo parametro
    if (argv[2][len - 1] == '\n') {
        argv[2][len - 1] = '\0';
    }

    // Verifico che il secondo parametro contenga l'estensione ".txt"
    if (strcmp(&argv[2][len - 4], ".txt")) {
        fprintf(stderr, "Error: output file %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // Alloco sull'heap uno spazio di memoria sufficiente a contenere un
    // cmd_input_t
    // NOTA: è necessario allocare tale struttura sull'heap in quanto, al
    // termine della funzione, tutti i dati allocati nello stack andranno
    // perduti.
    cmd_input_t *out = (cmd_input_t *)malloc(sizeof(cmd_input_t));

    // Copio i parametri passati al programma all'interno della struttura
    // NOTA: un'alternativa (più efficiente) potrebbe essere quella di salvare
    // il puntatore a tale stringa piuttosto che effettuare la copia dei
    // parametri.
    strncpy(out->input_file, argv[1], PATH_MAX);
    strncpy(out->output_file, argv[2], PATH_MAX);

    // restituisco il puntatore alla struttura precedentemente allocata
    return out;
}

lista readFile(char *input) {
    FILE *f;
    // Variabili temporanee per il parsing del file di input
    char nome[255];
    char cognome[255];
    int numero_conto;
    int anno_apertura;
    float saldo;
    // Imposto la lista a NULL in quanto questa è inizialmente vuota
    lista l = NULL;

    // Apro il file in sola lettura e verifico eventuali errori
    if ((f = fopen(input, "r")) == NULL) {
        fprintf(stderr, "Error: opening the input file %s\n", input);
        exit(EXIT_FAILURE);
    }

    // Leggo il file riga per riga e carico il contenuto in alcune variabili
    // temporanee
    while (fscanf(f, "%s %s %d %d %f", nome, cognome, &numero_conto,
                  &anno_apertura, &saldo) != EOF) {
        // Alloco sull'heap della memoria sufficiente a memorizzare un nuovo
        // cliente_t
        cliente_t *c = (cliente_t *)malloc(sizeof(cliente_t));

        // Imposto i dati del cliente_t
        strncpy(c->nome, nome, 255);
        strncpy(c->cognome, cognome, 255);
        c->numero_conto = numero_conto;
        c->anno_apertura = anno_apertura;
        c->saldo = saldo;

        // Inserisco il nuovo cliente all'interno della lista sfruttando la
        // funzione di inserimento implementata precedentemente.
        insertList(&l, c);
    }

    // Chiudo il file
    fclose(f);

    // Restituisco in output il puntatore alla lista
    return l;
}

// getMax con assunzione per la gestione degli errori (versione semplice)
float getMax(lista l) {
    float max = 0; // assumiamo che il saldo minimo sia 0 per gestire l'errore
    nodo_t *ptr = l;

    while (ptr != NULL) {
        if (ptr->cliente->saldo > max) {
            max = ptr->cliente->saldo;
        }

        ptr = ptr->next;
    }

    return max;
}

// getMax senza assunzione per la gestione degli errori.
// La funzione restituisce true se la lista è non vuota, false altrimenti.
// L'output della computazione viene effettivamente inserito all'interno dello
// spazio di memoria a cui punta max.
// bool getMax(lista l, float *max) {
//     if (l == NULL) {
//         return false;
//     }

//     nodo_t *ptr = l;
//     *max = ptr->cliente->saldo;

//     while (ptr != NULL) {
//         if (ptr->cliente->saldo > *max) {
//             *max = ptr->cliente->saldo;
//         }

//         ptr = ptr->next;
//     }

//     return true;
// }

// Calcola il valore di x così come specificato nella consegna del compito al
// punto C
float computeX(int anno_apertura, float saldo, float m) {
    float first_component = (2023 - anno_apertura) / 5.0;
    float second_component = saldo / m;

    if (first_component > 1) {
        return second_component;
    } else {
        return first_component * second_component;
    }
}

void removeAccount(lista *l) {
    // Se la lista è vuota termino la funzione: non vi è alcun elemento da
    // cancellare;
    if (*l == NULL) {
        return;
    }

    float x, x_tmp;
    // calcolo il saldo massimo presente nella lista
    float m = getMax(*l);

    nodo_t *to_remove;
    nodo_t *ptr = *l;

    // inizialmente imposto il valore di to_remove e x supponendo che l'elemento
    // da eliminare si trovi in testa
    to_remove = ptr;
    x = computeX(ptr->cliente->anno_apertura, ptr->cliente->saldo, m);
    ptr = ptr->next;

    // cerco il nodo con il massimo valore di X scorrendo la lista
    while (ptr != NULL) {
        // calcolo x per il nodo "visitato"
        x_tmp = computeX(ptr->cliente->anno_apertura, ptr->cliente->saldo, m);

        // verifico se il valore di x per il nodo che sto visitando è superiore
        // al massimo valore attuale
        if (x_tmp > x) {
            x = x_tmp;
            to_remove = ptr;
        }

        ptr = ptr->next;
    }

    // rimuovo il nodo
    removeNode(l, to_remove);
}

// La funzione scrive sul file indicato in input gli elementi presenti in lista
void writeFile(const lista l, char *output_file) {
    FILE *f;

    // Apro il file in sola scrittura e verifico non vi siano stati errori
    if ((f = fopen(output_file, "w")) == NULL) {
        fprintf(stderr, "Error: opening the output file %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    // scorro la lista e di volta in volta stampo le informazioni del cliente
    // sfruttando la funzione fprintCliente
    nodo_t *ptr = l;

    while (ptr != NULL) {
        fprintCliente(f, ptr->cliente);
        ptr = ptr->next;
    }

    // chiudo il file
    fclose(f);
}

int main(int argc, char **argv) {
    // Punto A
    cmd_input_t *cmd_input = readInput(argc, argv);
    // Punto B
    lista l = readFile(cmd_input->input_file);

    // Punto D
    unsigned h = getN(l) / 2;

    for (unsigned i = 0; i < h; i++) {
        removeAccount(&l);
    }

    // Punto E
    writeFile(l, cmd_input->output_file);

    // deallocazione dei dati presenti nell'heap
    destroyList(l);
    free(cmd_input);
}