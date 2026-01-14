#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int64_t map_[2][40][40];
int64_t random_seed_;
int64_t modulo_(int64_t a_, int64_t b_);
int64_t next_int_();
void create_initial_map_();
void print_map_(int64_t current_map_);
int64_t is_set_(int64_t row_, int64_t col_, int64_t current_map_);
int64_t number_of_neighbors_(int64_t row_, int64_t col_, int64_t current_map_);
void step_(int64_t current_map_);
int main(int argc, char **argv);

int64_t modulo_(int64_t a_, int64_t b_) {
  int64_t rem_;

  rem_ = a_ - b_ * (a_ / b_);
  if (rem_ < 0) {
    rem_ = rem_ + b_;
  }
  return rem_;
}

int64_t next_int_() {
  random_seed_ = 1103515245 * random_seed_ + 12345;
  return random_seed_;
}

void create_initial_map_() {
  int64_t row_;
  int64_t col_;
  int64_t b_;

  row_ = 0;
  while (row_ < 17) {
    col_ = 0;
    while (col_ < 40) {
      map_[0][row_][col_] = 0;
      col_ = col_ + 1;
    }
    row_ = row_ + 1;
  }
  b_ = 0;
  while (b_ < 4) {
    map_[0][2][3 + b_ * 9] = 1;
    map_[0][2][4 + b_ * 9] = 1;
    map_[0][2][5 + b_ * 9] = 1;
    map_[0][2][6 + b_ * 9] = 1;
    map_[0][2][7 + b_ * 9] = 1;
    map_[0][3][2 + b_ * 9] = 1;
    map_[0][3][7 + b_ * 9] = 1;
    map_[0][4][7 + b_ * 9] = 1;
    map_[0][5][2 + b_ * 9] = 1;
    map_[0][5][6 + b_ * 9] = 1;
    map_[0][6][4 + b_ * 9] = 1;
    b_ = b_ + 1;
  }
  while (row_ < 40) {
    col_ = 0;
    while (col_ < 40) {
      if (modulo_(next_int_(), 21) <= 5) {
        map_[0][row_][col_] = 1;
      } else {
        map_[0][row_][col_] = 0;
      }
      col_ = col_ + 1;
    }
    row_ = row_ + 1;
  }
}
void writeChar(int c) { putc(c, stdout); }
void print_line_() {
  int64_t col_;

  writeChar(43);
  col_ = 0;
  while (col_ < 40) {
    writeChar(45);
    writeChar(45);
    writeChar(45);
    col_ = col_ + 1;
  }
  writeChar(43);
  writeChar(10);
}

void print_map_(int64_t current_map_) {
  int64_t row_;
  int64_t col_;

  print_line_();
  row_ = 0;
  while (row_ < 40) {
    writeChar(124);
    col_ = 0;
    while (col_ < 40) {
      writeChar(32);
      if (map_[current_map_][row_][col_] == 1) {
        writeChar(88);
      } else {
        writeChar(32);
      }
      writeChar(32);
      col_ = col_ + 1;
    }
    writeChar(124);
    writeChar(10);
    row_ = row_ + 1;
  }
  print_line_();
}

int64_t is_set_(int64_t row_, int64_t col_, int64_t current_map_) {
  int64_t c_;

  if (row_ < 0 || row_ >= 40) {
    return 0;
  }
  c_ = modulo_(col_, 40);
  return map_[current_map_][row_][c_];
}

int64_t number_of_neighbors_(int64_t row_, int64_t col_, int64_t current_map_) {
  int64_t r_;
  int64_t c_;
  int64_t count_;

  count_ = 0;
  r_ = row_ - 1;
  while (r_ <= row_ + 1) {
    c_ = col_ - 1;
    while (c_ <= col_ + 1) {
      if (r_ != row_ || c_ != col_) {
        count_ = count_ + is_set_(r_, c_, current_map_);
      }
      c_ = c_ + 1;
    }
    r_ = r_ + 1;
  }
  return count_;
}

void step_(int64_t current_map_) {
  int64_t row_;
  int64_t col_;
  int64_t new_map_;

  new_map_ = modulo_(current_map_ + 1, 2);
  row_ = 0;
  while (row_ < 40) {
    col_ = 0;
    while (col_ < 40) {
      int64_t count_;

      count_ = number_of_neighbors_(row_, col_, current_map_);
      if (is_set_(row_, col_, current_map_) == 1) {
        if (count_ < 2 || count_ > 3) {
          map_[new_map_][row_][col_] = 0;
        } else {
          map_[new_map_][row_][col_] = 1;
        }
      } else {
        if (count_ == 3) {
          map_[new_map_][row_][col_] = 1;
        } else {
          map_[new_map_][row_][col_] = 0;
        }
      }
      col_ = col_ + 1;
    }
    row_ = row_ + 1;
  }
  return;
}

static size_t readInt() {
  char line[1024];
  size_t len;
  int res = 0;
  fgets(line, 1024, stdin);
  res = strtol(line, NULL, 10);
  return res;
}
int main(int argc, char **argv) {
  int64_t current_step_;
  int64_t current_map_;
  int64_t it_count_;
  random_seed_ = readInt();
  it_count_ = readInt();
  create_initial_map_();
  current_map_ = 0;
  print_map_(current_map_);

  for (current_step_ = 0; current_step_ < it_count_; current_step_++) {
    step_(current_map_);
    current_map_ = modulo_(current_map_ + 1, 2);
    current_step_ = current_step_ + 1;
  }
  print_map_(current_map_);
  return 0;
}
