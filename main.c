#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "organism.h"

#define BUFF_LEN 13
#define MAX_POPULATION 1000
#define HALF_POPULATION MAX_POPULATION / 2

static int __calculate_fitness(char *source, char *target);

static void __mutate(char *source, int number_of_characters);

static struct organism **__create_population(size_t population_size,
                                             size_t dna_len, char *target_dna);

static struct organism *__select_random_parent(struct organism **population,
                                               size_t population_size);

static int __sort_compare(const void *org1, const void *org2);

int main() {
  srand(time(NULL));

  struct organism **population;
  struct organism *parent1, *parent2, *child;
  char *target = "Hello, World!";
  int generation, child_cnt;
  bool all_evolved = true;

  population = __create_population(MAX_POPULATION, BUFF_LEN, target);

  qsort(population, MAX_POPULATION, sizeof(struct organism *),
        __sort_compare);

  printf("Initial Population:\n");
  for (int i = 0; i < MAX_POPULATION; i++) {
    organism_print(population[i]);
    printf("\n");
  }
  printf("\n");

  generation = 0;

  while (true) {

    all_evolved = true;
    for (int i = 0; i < MAX_POPULATION; i++) {
      if (population[i]->fitness_score != 0) {
        all_evolved = false;
        break;
      }
    }

    if (all_evolved) {
      printf("\nWinner! Generation %i\n", generation);
      break;
    }

    child_cnt = 0;
    generation++;

    /*
     * Since there's a sort happening, I'm doing some pretty serious elitism
     * by selecting only the first quarter of the population to mate. This
     * yields great results faster by only force feeding the population with
     * very good children.
     */
    for (int i = 0; i < HALF_POPULATION / 2; i++) {
      parent1 = population[i];
      parent2 = population[i + 1];

      child = organism_create_child_with(parent1, parent2);
      organism_calculate_fitness(child, target, __calculate_fitness);

      if (child->fitness_score != 0) {
        organism_mutate(child, __mutate);
        organism_calculate_fitness(child, target, __calculate_fitness);
      }

      if (organism_compare_to(child, population[(MAX_POPULATION - 1) - i]) < 0) {
        organism_destroy_organism(population[(MAX_POPULATION - 1) - i]);
        population[(MAX_POPULATION - 1) - i] = NULL;
        population[(MAX_POPULATION - 1) - i] = child;

        child_cnt++;
      } else {
        organism_destroy_organism(child);
      }
    }

    // Only sort if children were added this generation
    if (child_cnt > 0) {
      qsort(population, MAX_POPULATION, sizeof(struct organism *),
            __sort_compare);
    }

#ifdef DEBUG
    printf("Generation %i\n", generation);
    for (int i = 0; i < MAX_POPULATION; i++) {
      printf("Gen %i; ", generation);
      organism_print(population[i]);
      printf("\n");
    }
    printf("\n");
#endif
  }

  // Unnecessary cleanup for valgrind to be happy :)
  for (int i = 0; i < MAX_POPULATION; i++) {
    organism_destroy_organism(population[i]);
  }

  free(population);

  return EXIT_SUCCESS;
}

static int __calculate_fitness(char *source, char *target) {
  int fitness_score = 0;
  size_t source_len = strlen(source);
  size_t target_len = strlen(target);
  assert(source_len == target_len);

  for (size_t i = 0; i < source_len; i++) {
    fitness_score +=
        (int) pow((double) ((int) source[i] - (int) target[i]), (double) 2);
  }

  return fitness_score;
}

static void __mutate(char *source, int number_of_characters) {
  size_t source_len = strlen(source);

  if (number_of_characters < 1 || number_of_characters > source_len) {
    number_of_characters = 1;
  }

  for (int i = 0; i < number_of_characters; i++) {
    int characer_index = rand() % (int) source_len;
    source[characer_index] =
        (char) ((int) source[characer_index] + ((rand() % 2) ? 1 : -1));
  }

}

static struct organism **__create_population(size_t population_size,
                                             size_t dna_len,
                                             char *target_dna) {
  struct organism **population;

  population = calloc(population_size, sizeof(struct organism *));

  for (int i = 0; i < population_size; i++) {
    population[i] = organism_create_with_random_dna(dna_len);
    organism_calculate_fitness(population[i], target_dna, __calculate_fitness);
  }

  return population;
}

static struct organism *__select_random_parent(struct organism **population,
                                               size_t population_size) {
  double rand_one = (double) rand() / (double) ((unsigned) RAND_MAX + 1);
  double rand_two = (double) rand() / (double) ((unsigned) RAND_MAX + 1);

  int index = (int) (rand_one * rand_two * (double) (population_size - 1));
  return population[index];
}

static int __sort_compare(const void *org1, const void *org2) {
  return organism_compare_to(*(struct organism **) org1,
                             *(struct organism **) org2);
}