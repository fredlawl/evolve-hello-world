
#ifndef HELLOWORLD_ORGANISM_H
#define HELLOWORLD_ORGANISM_H

#include <stdlib.h>

struct organism {
  size_t dna_len;
  char *dna;
  int fitness_score;
};

struct organism *organism_create_with_random_dna(size_t dna_len);

struct organism *organism_create_from_dna(const char *dna, size_t dna_len);

void organism_destroy_organism(struct organism *organism);

void organism_calculate_fitness(struct organism *org, char *target_dna,
                                int (*fitness_func)(char *, char *));

void organism_mutate(struct organism *org, void (*mutate_func)(char *, int));

int organism_compare_to(const struct organism *a, const struct organism *b);

struct organism *organism_create_child_with(const struct organism *organism,
                                            const struct organism *mate);

static inline void organism_print(const struct organism *org) {
  printf("%zu; %s; score: %i", org->dna_len, org->dna, org->fitness_score);
}

#endif
