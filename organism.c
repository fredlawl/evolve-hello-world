#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>

#include "organism.h"

struct organism *organism_create_from_dna(const char *dna, size_t dna_len) {
  struct organism *org;

  assert(strlen(dna) == dna_len);

  if (!(org = (struct organism *) malloc(sizeof(struct organism)))) {
    return NULL;
  }

  org->dna_len = dna_len;
  org->dna = (char *) malloc(sizeof(char) * (dna_len + 1));
  org->dna[0] = '\0';
  strcpy(org->dna, dna);
  org->fitness_score = INT_MAX;

  return org;
}

struct organism *organism_create_with_random_dna(size_t dna_len) {
  struct organism *org;
  char *dna;
  int rand_num;

  dna = (char *) malloc(sizeof(char) * (dna_len + 1));
  for (int i = 0; i < dna_len; i++) {
    rand_num = (rand() % (127 - 32)) + 32; // full range
    //rand_num = (rand() % (122 - 65)) + 65; // alpha + some special chars

    dna[i] = (char) rand_num;
  }

  dna[dna_len] = '\0';

  org = organism_create_from_dna(dna, dna_len);

  free(dna);

  return org;
}

void organism_destroy_organism(struct organism *organism) {
  if (organism->dna)
    free(organism->dna);
  free(organism);
}

void organism_calculate_fitness(struct organism *org, char *target_dna,
                                int (*fitness_func)(char *, char *)) {
  org->fitness_score = fitness_func(org->dna, target_dna);
}

int organism_compare_to(const struct organism *a, const struct organism *b) {
  if (a->fitness_score == b->fitness_score)
    return 0;
  return (a->fitness_score < b->fitness_score) ? -1 : 1;
}

struct organism *organism_create_child_with(const struct organism *organism,
                                            const struct organism *mate) {
  char *child_dna;
  struct organism *child;
  int half, start, stop;

  assert(organism->dna_len == mate->dna_len);

  child = NULL;
  child_dna = (char *) malloc(sizeof(char) * (organism->dna_len + 1));
  child_dna[0] = '\0';
  strcpy(child_dna, organism->dna);

  half = ((int) organism->dna_len / 2);

  if (organism_compare_to(organism, mate) >= 0) {
    start = 0;
    stop = half;
  } else {
    start = half;
    stop = (int) organism->dna_len;
  }

  strncpy(child_dna + start, mate->dna + start, stop - start);

  child = organism_create_from_dna(child_dna, organism->dna_len);
  free(child_dna);

  return child;
}

void organism_mutate(struct organism *org, void (*mutate_func)(char *, int)) {
  mutate_func(org->dna, 3);
}