/* PCP Solver

* reads the following format:
* num_words
* wordA0;wordA1;wordA2;...;
* wordB0;wordB1;wordB2;...; */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int bool;
static const int true = 1;
static const int false = 0;
static const int top = 1;
static const int bottom = 0;
typedef struct Domino {
  char *top;
  size_t n_top;
  char *bot;
  size_t n_bot;
} Domino;
typedef struct Dominoes {
  Domino *problem;
  size_t n_words;
} Dominoes;
typedef struct Overhang {
  char *overhang;
  int overhang_side;
  size_t overhang_size;
} Overhang;

static int readInt() {
  char line[1024];
  size_t len;
  int res = 0;
  fgets(line, 1024, stdin);
  res = strtol(line, NULL, 10);
  return res;
}

// #define DEBUG
static Dominoes readProblem() {
  int num_words = readInt();
  Domino *prob = (Domino *)malloc(sizeof(Domino) * num_words);
  // first read top strings
  for (int i = 0; i < num_words; i++) {
    char *line;
    size_t n = 0;
    n = getdelim(&line, &n, ';', stdin);
    prob[i].n_top = n - 1;
    line[n - 1] = '\0';
    prob[i].top = line;
  }
  getc(stdin); // newline
  // then the bottom strings
  for (int i = 0; i < num_words; i++) {
    char *line;
    size_t n = 0;
    n = getdelim(&line, &n, ';', stdin);
    prob[i].n_bot = n - 1;
    line[n - 1] = '\0';
    prob[i].bot = line;
  }
  Dominoes res;
  res.problem = prob;
  res.n_words = num_words;
#ifdef DEBUG
  printf("================\nProblem statement (size: %d):\n", (int)res.n_words);
  for (int i = 0; i < res.n_words; i++) {
    printf("%c (%s,%d)|(%s,%d)", i == 0 ? ' ' : ',', res.problem[i].top,
           (int)res.problem[i].n_top, res.problem[i].bot,
           (int)res.problem[i].n_bot);
  }
  printf("\n================\n");
#endif
  return res;
}

static void freeProblem(Dominoes *prob) {
  for (int i = 0; i < prob->n_words; i++) {
    free(prob->problem[i].top);
    free(prob->problem[i].bot);
  }
  free(prob->problem);
}
static bool isPrefixDir(const char *a, const char *b) {
  if (!a || !b)
    return true;
  for (int i = 0; a[i] != '\0' && b[i] != '\0'; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
static bool isPrefix(const Overhang *overhang, const Domino *dom) {
  const char *other_string =
      overhang->overhang_side == top ? dom->bot : dom->top;
  const bool simple = isPrefixDir(other_string, overhang->overhang);
  if ((overhang->overhang_side == top ? dom->n_bot : dom->n_top) <=
      overhang->overhang_size) {
    return simple;
  } else if (simple) {
    // match remaining
    const char *same_string =
        overhang->overhang_side == top ? dom->top : dom->bot;
    return isPrefixDir(&other_string[overhang->overhang_size], same_string);
  }
  return false;
}

static Overhang fromEmpty(const Domino *first_domino) {
  Overhang res;
  res.overhang_side = first_domino->n_top > first_domino->n_bot ? top : bottom;
  res.overhang_size = res.overhang_side == top
                          ? first_domino->n_top - first_domino->n_bot
                          : first_domino->n_bot - first_domino->n_top;
  res.overhang = malloc(sizeof(char) * (res.overhang_size + 1));
  size_t start_read =
      res.overhang_side == top ? first_domino->n_bot : first_domino->n_top;
  memcpy(res.overhang,
         &(res.overhang_side == top ? first_domino->top
                                    : first_domino->bot)[start_read],
         res.overhang_size);
  res.overhang[res.overhang_size] = '\0';
#ifdef DEBUG
  printf("%s on %d with length %d\n", res.overhang, res.overhang_side,
         (int)res.overhang_size);
#endif
  return res;
}

static Overhang updateOverhang(const Overhang *curr, const Domino *new_domino) {
  Overhang res;
#ifdef DEBUG
  printf("%s (on %d) + (%s|%s) = ", curr->overhang, curr->overhang_side,
         new_domino->top, new_domino->bot);
#endif
  size_t n_same =
      curr->overhang_side == top ? new_domino->n_top : new_domino->n_bot;
  size_t n_other =
      curr->overhang_side == top ? new_domino->n_bot : new_domino->n_top;
  char *s_same = curr->overhang_side == top ? new_domino->top : new_domino->bot;
  char *s_other =
      curr->overhang_side == top ? new_domino->bot : new_domino->top;
  if (curr->overhang_size + n_same >= n_other) { // no switch, overhang stays
    // new overhang is old overhang minus the prefix from the other side plus
    // the new domino
    res.overhang_size = curr->overhang_size + n_same - n_other;
    res.overhang_side = curr->overhang_side;
    if (res.overhang_size == 0) {
      res.overhang = NULL;
      return res;
    }
    res.overhang = malloc(sizeof(char) * (res.overhang_size + 1));
    long remaining_overhang = (long)curr->overhang_size - (long)n_other;
    if (remaining_overhang > 0) {
      memcpy(res.overhang,
             &curr->overhang[curr->overhang_size - remaining_overhang],
             remaining_overhang);
      memcpy(&res.overhang[remaining_overhang], s_same, n_same);
    } else {
      memcpy(res.overhang, &s_same[-remaining_overhang],
             n_same + remaining_overhang);
    }
  } else { // switch sides
    // new overhang is other side minus the combined size of existing overhang +
    // same side
    res.overhang_size = n_other - n_same - curr->overhang_size;
    res.overhang_side = curr->overhang_side == top ? bottom : top;
    res.overhang = malloc(sizeof(char) * (res.overhang_size + 1));
    memcpy(res.overhang, &s_same[curr->overhang_size + n_same],
           res.overhang_size);
  }
  res.overhang[res.overhang_size] = '\0';
#ifdef DEBUG
  printf("%s on %d with length %d\n", res.overhang, res.overhang_side,
         (int)res.overhang_size);
#endif
  return res;
}

static int max_depth = 1;
static int *solution;
static bool solve(const Dominoes *prob, Overhang *overhang, int depth) {
  if (overhang->overhang_size == 0) {
    return true;
  }
  if (depth >= max_depth) {
    return false;
  }
  for (size_t d = 1; d < prob->n_words; d++) {
    // is d viable?
    if (isPrefix(overhang, &prob->problem[d])) {
      Overhang new_overhang = updateOverhang(overhang, &prob->problem[d]);
      solution[depth] = d;
      bool success = solve(prob, &new_overhang, depth + 1);
      if (new_overhang.overhang)
        free(new_overhang.overhang);
      if (success) {
        return true;
      }
    }
  }
  return false;
}
static void printSolution(Dominoes *prob) {
  printf("================\nFound solution with length %d:\n0", max_depth);
  for (int i = 0; i < max_depth; i++) {
    printf(", %d", solution[i]);
  }
  printf("\n");
}
int main() {
  Dominoes prob = readProblem();
  Overhang initial = fromEmpty(&prob.problem[0]);
  bool solved = false;
  solution = malloc(sizeof(int) * max_depth);
  while (max_depth < 1000 && !solved) {
#ifdef DEBUG
    printf("max depth: %d\n", max_depth);
#endif
    solution = realloc(solution, sizeof(int) * max_depth);
    solved |= solve(&prob, &initial, 0);
    max_depth++;
  }
  if (solved) {
    printSolution(&prob);
  } else {
    printf("No solution with a maximum depth of %d\n", max_depth);
  }
  free(initial.overhang);
  free(solution);
  freeProblem(&prob);
}
