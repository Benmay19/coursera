#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_VERTICES    201

#define MAX_CUT_TESTS   100000

typedef struct _vertex {
    int  id;
    int  degree;
    int  neighbours[MAX_VERTICES];
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

static void print_vertex(Vertex *G, Vertex *V)
{
    printf("\t Node %i (%i degrees):", V->id, V->degree);
    for (int i = 1; i < MAX_VERTICES; i++) {
        // if (G[i].neighbours[j])
            printf("\t%i: %i", G[i].id, V->neighbours[i]);
    }
    printf("\n");
}

static void print_graph(Vertex *G)
{
    for (int i = 1; i < MAX_VERTICES; i++) {
        print_vertex(G, &G[i]);
    }
}

static int min_cut(Vertex *G)
{
    int v = MAX_VERTICES - 1;

    // fold!
    while (v > 2) {
        // pick random vertex
        Vertex *U = NULL;
        int r_U = rand() % v + 1;
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

        // check edge is not a loop
        if (U->id == V->id) {
            fprintf(stderr, "Edge %i, %i is a loop\n", U->id, V->id);
            exit(1);
        }

        // check edge is balanced
        if (U->neighbours[V->id] != V->neighbours[U->id]) {
            fprintf(stderr, "Edge %i, %i is unbalanced: %i, %i\n", U->id, V->id, U->neighbours[V->id], V->neighbours[U->id]);
            exit(1);
        }

        // delete selected edge and parallels that cause self-loops
        U->degree -= U->neighbours[V->id];
        U->neighbours[V->id] = 0;

        V->degree -= V->neighbours[U->id];
        V->neighbours[U->id] = 0;

        // check edge is still balanced
        if (U->neighbours[V->id] != V->neighbours[U->id]) {
            fprintf(stderr, "Edge %i, %i is unbalanced after deletion: %i, %i\n", U->id, V->id, U->neighbours[V->id], V->neighbours[U->id]);
            exit(1);
        }

        // move edges from U to V
        for (int i = 1; i < MAX_VERTICES; i++) {
            if (0 < U->neighbours[i] && i != U->id && i != V->id) {
                // check edge is balanced
                if (U->neighbours[i] != G[i].neighbours[U->id]) {
                    fprintf(stderr, "Edge %i, %i is unbalanced: %i, %i\n", U->id, i, U->neighbours[i], G[i].neighbours[U->id]);
                    exit(1);
                }

                // add edges to V
                V->neighbours[i] += U->neighbours[i];
                V->degree += U->neighbours[i];

                // update the adjacent vertices
                G[i].neighbours[V->id] += G[i].neighbours[U->id];
                G[i].neighbours[U->id] = 0;

                // remove edges from U
                U->degree -= U->neighbours[i];
                U->neighbours[i] = 0;
            }
        }

        // validate V and U
        if (U->degree != 0) {
            fprintf(stderr, "Collapsed vertex %i in (%i, %i) has %i degrees\n", U->id, U->id, V->id, U->degree);
            print_vertex(G, U);
            exit(1);
        }

        v--;
    }

    // count edges on minimum cut
    for (int i = 1; i < MAX_VERTICES; i++)
        if (G[i].degree)
            return G[i].degree;

    return -1;
}

int main(int argc, char *argv[])
{
    time_t t;

    // seed rand
    srand((unsigned) time(&t));

    // read graph
    Vertex *G = read_vertices(stdin);

    // test MAX_CUT_TESTS times
    int min = 0xFFFF;
    int test = 0;
    for (int i = 0; i < MAX_CUT_TESTS; i++) {
        // copy graph
        Vertex *G2 = calloc(MAX_VERTICES, sizeof(Vertex));
        memcpy(G2, G, sizeof(Vertex) * MAX_VERTICES);

        // get min cut
        if (min > (test = min_cut(G2)))
            min = test;

        free(G2);
    }
    
    printf("Minimum cut edges: %i\n", min);

    free(G);

    return 0;

}
