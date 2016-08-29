#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_VERTICES    201

typedef struct _vertex {
    int  id;
    int  degree;
    char neighbours[MAX_VERTICES];
} Vertex;

static Vertex *read_vertices(FILE *f)
{
    size_t      sz      = 0;
    char        *c      = NULL;
    char        *p      = NULL;
    int         clen    = 0;
    long int    n       = 0;
    long int    v       = 0;

    // allocate
    Vertex *G = calloc(MAX_VERTICES, sizeof(Vertex));
    for (int i = 0; i < MAX_VERTICES; i++) {
        for (int j = 0; j < MAX_VERTICES; j++) {
            G[i].id = 0;
            G[i].degree = 0;
            G[i].neighbours[j] = 0;
        }
    }

    // read lines
    while (-1 < getline(&c, &sz, f)) {
        v = strtol(c, &p, 10);
        G[v].id = v;
        
        while (0 < (n = strtol(p, &p, 10))) {
            G[v].neighbours[n]++;
            G[v].degree++;
        }

        free(c);
        c = NULL;
        p = NULL;
        sz = 0;
    }

    return G;
}

static void print_graph(Vertex *G) {
    for (int i = 1; i < MAX_VERTICES; i++) {
        printf("\t Node %i (%i degrees):", G[i].id, G[i].degree);
        for (int j = 1; j < MAX_VERTICES; j++) {
            // if (G[i].neighbours[j])
                printf("\t%i: %i", G[j].id, G[i].neighbours[j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    Vertex *G = read_vertices(stdin);
    int v = MAX_VERTICES - 1;
    time_t t;

    // seed rand
    srand((unsigned) time(&t));

    printf("===\nInput:\n===\n");
    //print_graph(G);

    // fold!
    while (v > 2) {
        printf("v: %i\n", v);

        // pick random vertex
        Vertex *U = NULL;
        int r_U = rand() % v + 1;

        printf("Selecting vertex %i of %i\n", r_U, v);

        for (int i = 1; i < MAX_VERTICES; i++){
            if (0 < G[i].degree)
                r_U--;

            if (0 >= r_U) {
                U = &G[i];
                break;
            }
        }

        // count neighbours to U
        int n_V = 0;
        for (int i = 1; i < MAX_VERTICES; i++)
            if (U->neighbours[i])
                n_V++;


        // pick a random edge
        Vertex *V = NULL;
        int r_V = rand() % n_V + 1;

        printf("Selecting edge %i of %i on vertex (%i)\n", r_V, n_V, U->id);

        for (int i = 1; i < MAX_VERTICES; i++){
            if (i != U->id) {
                if (0 < U->neighbours[i])
                    r_V--;

                if (0 >= r_V) {
                    V = &G[i];
                    break;
                }
            }
        }


        printf("Selected nodes %i and %i\n", U->id, V->id);

        // delete selected edge and parallels that cause self-loops
        U->degree -= U->neighbours[V->id];
        U->neighbours[V->id] = 0;

        V->degree -= V->neighbours[U->id];
        V->neighbours[U->id] = 0;

        printf("===\nDeleted edge %i, %i:\n===\n", U->id, V->id);
        //print_graph(G);


        // copy edges from U to V
        for (int i = 1; i < MAX_VERTICES; i++) {
            if (0 < U->neighbours[i]) {
                // link V to edges from U
                if (i != U->id) {
                    V->neighbours[i] += U->neighbours[i];
                    V->degree += U->neighbours[i];
                }

                // link edges from U to V
                G[i].neighbours[V->id] += G[i].neighbours[U->id];
                G[i].neighbours[U->id] = 0;

                U->degree -= U->neighbours[i];
                U->neighbours[i] = 0;
            }
        }

        printf("===\nMoved edges from %i to %i:\n===\n", U->id, V->id);
        //print_graph(G);

        v--;
    }

    printf("===\nMin cut:\n===\n");
    print_graph(G);

    free(G);

    return 0;

}
