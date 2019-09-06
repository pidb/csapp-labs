#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>

#define ADDR_BITS 64
#define MAX_NAME_LEN 1000

#define TAG(addr, s, b) (addr >> (s + b))
#define SET(addr, s, b) ((addr >> b) & ((uint64_t)(-1) >> (ADDR_BITS - s)))

typedef struct cache_line {
    uint64_t tag;
    unsigned lru;
    bool val;
} cache_line;

typedef cache_line ** Cache;

void parse_args(
    int argc, char *argv[],
    int *s, int *E, char *t, int *b, bool *v)
{
    int opt = 0;
    while ((opt = getopt(argc, argv, "s:E:t:b:v")) != -1) {
        switch (opt) {
        case 's':
            *s = atoi(optarg);
            break;
        case 'E':
            *E = atoi(optarg);
            break;
        case 't': 
            strcpy(t, optarg);
            break;
        case 'b':
            *b = atoi(optarg);
            break;
        case 'v':
            *v = true;
            break;
        default:
            abort();
        }
    }
}

int get_lru_line(Cache cache, int set, int E)
{
    unsigned lru_idx, min_lru = UINT_MAX, lru;
    for (int ln = 0; ln < E; ++ln) {
        lru = cache[set][ln].lru;
        if (lru < min_lru) {
            min_lru = lru;
            lru_idx = ln;
        }
    }
    return lru_idx;
}

void process_line(
    int time,
    Cache cache, 
    char identifier, uint64_t addr, int size,
    int b, int E, int s,
    bool *out_first, bool *out_evict, bool *out_second)
{   
    uint64_t tag = TAG(addr, s, b);
    uint64_t set = SET(addr, s, b);

    bool first = false, eviction = false, second = false;
    int free_ln = -1;

    for (int ln = 0; ln < E; ++ln) {
        bool ln_val = cache[set][ln].val;
        uint64_t ln_tag = cache[set][ln].tag;

        if (ln_val) {
            if (ln_tag == tag) { // hit
                first = true;
                cache[set][ln].lru = time;
                free_ln = ln;
                break;
            }
        } else {
            free_ln = ln;
        }
    }

    if (!first) { // miss
        if (free_ln != -1) { // no eviction
            cache[set][free_ln].val = true;
            cache[set][free_ln].tag = tag;
            cache[set][free_ln].lru = time;
        } else { // eviction
            eviction = true;
            free_ln = get_lru_line(cache, set, E);
            cache[set][free_ln].tag = tag;
            cache[set][free_ln].lru = time;
        }
    }

    // 'M' special case.
    if (identifier == 'M') {
        second = true;
        cache[set][free_ln].lru = time + 1; // store
    }

    *out_first = first;
    *out_evict = eviction;
    *out_second = second;
}

void print_line(char identifier, uint64_t addr, int size,
    bool first, bool eviction, bool second)
{
    printf("%c %lx,%d ", identifier, addr, size);

    if (first) { printf("hit "); }
    else       { printf("miss "); }

    if (eviction) { printf("eviction "); }

    if (identifier != 'M') {
        printf("\n");
    } else {
        if (second) { printf("hit\n"); }
        else        { printf("miss\n"); }
    }
}

void process_trace(
    char *filepath,
    Cache cache,
    int *hits, int *misses, int *evictions,
    int b, int E, int s, bool v)
{
    FILE *pfile;
    pfile = fopen(filepath, "r");

    char identifier;
    uint64_t addr;
    int size;

    bool first, eviction, second;
    int time = 0;
    while (fscanf(pfile, " %c %lx,%d", &identifier, &addr, &size) != EOF) {
        if (identifier == 'I') { continue; }

        first = false; eviction = false; second = false;

        process_line(time, cache, identifier, addr, size, b, E, s,
            &first, &eviction, &second);

        *hits += (int) first + (int) second;
        *misses += (int) (!first);
        *evictions += (int) eviction;

        time++;
        
        if (v) {
            print_line(identifier, addr, size, first, eviction, second);
        }
    }

    fclose(pfile);
}

Cache init_cache(int E, int S)
{
    Cache cache; 
    cache = malloc(S * sizeof(cache_line *));
    if (!cache) { abort(); }

    for (int i = 0; i < S; ++i) {
        cache[i] = malloc(E * sizeof(cache_line));
        if (!cache[i]) { abort(); }
    }

    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < E; ++j) {
            cache[i][j].val = false;
            cache[i][j].tag = 0;
            cache[i][j].lru = 0;
        }
    }

    return cache;
}

void free_cache(Cache cache, int S)
{
    for(int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
}

int main(int argc, char *argv[]) 
{
    int s = 0, E = 0, b = 0;
    bool v = false;
    char trace_path[MAX_NAME_LEN];

    parse_args(argc, argv, &s, &E, trace_path, &b, &v);
    int S = pow(2, s);

    Cache cache = init_cache(E, S);

    int hits = 0, misses = 0, evictions = 0;
    process_trace(trace_path, cache, &hits, &misses, &evictions, b, E, s, v);

    printSummary(hits, misses, evictions);

    free_cache(cache, S);

    return 0;
}
