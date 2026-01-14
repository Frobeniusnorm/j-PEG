#include <gmp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void calculate_pi_chudnovsky(mpf_t pi, unsigned long digits) {
  // bits ≈ digits * log2(10). Add a safety margin.
  const double LOG2_10 = 3.32192809488736234787;
  mp_bitcnt_t prec_bits = (mp_bitcnt_t)ceil(digits * LOG2_10) + 128;
  mpf_set_default_prec(prec_bits);

  const unsigned long C = 640320UL;
  // Each term of the "S" series in the common Chudnovsky form uses (-C)^(3k)
  // Final formula: pi = 426880 * sqrt(10005) / S
  mpf_t sumS, termM, Ak, Rk, tmp, c3_f, sqrt10005, numer, denom;
  mpf_inits(sumS, termM, Ak, Rk, tmp, c3_f, sqrt10005, numer, denom, NULL);

  // c3 = C^3 as mpf
  // 640320^3 fits in 64-bit, but use mpf for generality.
  mpf_set_ui(tmp, C);
  mpf_mul(c3_f, tmp, tmp);  // C^2
  mpf_mul(c3_f, c3_f, tmp); // C^3

  // sqrt(10005)
  mpf_set_ui(tmp, 10005UL);
  mpf_sqrt(sqrt10005, tmp);

  // Each term adds ~14.181647462 digits.
  const double DIGITS_PER_TERM = 14.181647462;
  unsigned long terms = (unsigned long)ceil((digits / DIGITS_PER_TERM)) + 2UL;
  if (terms < 1UL)
    terms = 1UL;
  mpf_set_ui(termM, 1UL);
  mpf_set_ui(sumS, 0UL);

  for (unsigned long k = 0; k < terms; ++k) {
    // A_k = 13591409 + 545140134 * k
    mpf_set_ui(Ak, 545140134UL);
    mpf_mul_ui(Ak, Ak, k);
    mpf_add_ui(Ak, Ak, 13591409UL);

    // sum += A_k * M_k
    mpf_mul(tmp, Ak, termM);
    mpf_add(sumS, sumS, tmp);
    // Prepare next M_{k+1}: 
    // M_{k+1} = M_k * [ - (6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
    //                   / ( (k+1)^3 (3k+1)(3k+2)(3k+3) * C^3 ) ]
    // Skip update after last iteration
    if (k + 1 < terms) {
      mpf_set_si(numer, -1);
      for (int i = 1; i <= 6; ++i) {
        mpf_mul_ui(numer, numer, 6UL * k + (unsigned long)i);
      }
      // denom = (k+1)^3 * (3k+1)(3k+2)(3k+3) * C^3
      mpf_set_ui(denom, (k + 1));
      mpf_mul_ui(denom, denom, (k + 1));
      mpf_mul_ui(denom, denom, (k + 1)); // (k+1)^3
      mpf_mul_ui(denom, denom, 3UL * k + 1UL);
      mpf_mul_ui(denom, denom, 3UL * k + 2UL);
      mpf_mul_ui(denom, denom, 3UL * k + 3UL);
      mpf_mul(denom, denom, c3_f); // * C^3
      // Rk = numer / denom
      mpf_div(Rk, numer, denom);
      // M_{k+1} = M_k * Rk
      mpf_mul(termM, termM, Rk);
    }
  }
  // pi = 426880 * sqrt(10005) / S
  mpf_mul_ui(tmp, sqrt10005, 426880UL);
  mpf_div(pi, tmp, sumS);
  mpf_clears(sumS, termM, Ak, Rk, tmp, c3_f, sqrt10005, numer, denom, NULL);
}

static int readInt() {
  char line[1024];
  size_t len;
  int res = 0;
  fgets(line, 1024, stdin);
  res = strtol(line, NULL, 10);
  return res;
}

int main(int argc, char **argv) {
  unsigned long digits = readInt();
  mpf_set_default_prec((mp_bitcnt_t)ceil(digits * 3.32193) + 128);
  mpf_t pi;
  mpf_init(pi);
  calculate_pi_chudnovsky(pi, digits);
  gmp_printf("Pi = %.*Ff\n", (int)digits, pi);
  mpf_clear(pi);
  return 0;
}
