#include "numtheory.h"
#include "randstate.h"

//function could give issues because setting a and b in function
void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t temp, temp_a, temp_b;
	mpz_init(temp);
	mpz_init_set(temp_a, a);
	mpz_init_set(temp_b, b);

	while (mpz_cmp_ui(temp_b, 0) != 0) {
		mpz_set(temp, temp_b);
		mpz_mod(temp_b, temp_a, temp_b);
		mpz_set(temp_a, temp);
	}
	mpz_set(d, temp_a);
	mpz_clears(temp, temp_a, temp_b, NULL);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n){
	mpz_t r, rp, t, tp, q, temp;
	mpz_inits(r, rp, t, tp, q, temp, NULL);
	mpz_set_ui(t, 0);
	mpz_set_ui(tp, 1);
	mpz_set(r, n);
	mpz_set(rp, a);

	while (mpz_cmp_ui(rp, 0) != 0) {
		mpz_set(temp, rp);
		mpz_fdiv_q(q, r, rp);
		mpz_mul(rp, q, rp);
		mpz_sub(rp, r, rp);
		mpz_swap(r, temp);
		
		mpz_set(temp, tp);
		mpz_mul(tp, q, tp);
		mpz_sub(tp, t, tp);
		mpz_swap(t, temp);
	}
	if (mpz_cmp_ui(r, 1) > 0) {
		printf("No inverse found\n");
		mpz_set_ui(o, 0);
		mpz_clears(r, rp, t, tp, q, temp, NULL);
		return;
	}
	if (mpz_cmp_ui(t, 0) < 0) {
		mpz_add(t, t, n);
	}
	mpz_set(o, t);
	mpz_clears(r, rp, t, tp, q, temp, NULL);
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n){
	mpz_t v, p, temp_d;
	mpz_inits(v, p, temp_d, NULL);
	mpz_set_ui(v, 1);
	mpz_set(p, a);
	mpz_set(temp_d, d);

	while (mpz_cmp_ui(temp_d, 0) > 0) {
		if (mpz_tstbit(temp_d, 0) == 1) {
			mpz_mul(v, v, p);
			mpz_mod(v, v, n);
		}
		mpz_mul(p, p, p);
		mpz_mod(p, p, n);
		mpz_fdiv_q_ui(temp_d, temp_d, 2);
	}
	mpz_set(o, v);
	mpz_clears(v, p, temp_d, NULL);
}

bool is_prime(mpz_t n, uint64_t iters){
	//n-1=2^s*r s.t. r is odd
	// If I have time reformate while loop since this is janky ******************************
	uint64_t s = 1;
	mpz_t base, even_n, r, temp, a, y, base_2;
	mpz_init_set_ui(base, 2);
	mpz_inits(even_n, r, temp, y, a, NULL);

	mpz_set(even_n, n);
	mpz_sub_ui(even_n, even_n, 1);

	int x;
	while ((x = mpz_divisible_p(even_n, base)) != 0) {
		mpz_mul_ui(base, base, 2);
		s += 1;
	}
	mpz_fdiv_q_ui(base, base, 2);
	s -= 1;
	mpz_cdiv_q(r, even_n, base);
	//end part 1******************************************************************************
	uint64_t j;

	mpz_set(temp, n);
	mpz_sub_ui(temp, temp, 2);

	mpz_init_set_ui(base_2, 2);
	for (uint64_t i = 0; i < iters; i++) {
		uint64_t temp_a = random();
		if (mpz_cmp_ui(temp, temp_a) < 0) {
			uint64_t utemp = mpz_get_ui(temp);
			temp_a = temp_a % utemp;
		}
		if (temp_a <= 2) {
			temp_a += 2;
		}
		mpz_set_ui(a, temp_a);
		pow_mod(y, a, r, n);
		if ((x = mpz_cmp_d(y, 1)) != 0 && (x = mpz_cmp(even_n, y)) != 0) {
			j = 1;
			while (j <= s - 1 && mpz_cmp(y, even_n) != 0) {
				pow_mod(y, y, base_2, n);
				if (mpz_cmp_d(y, 1) == 0) {
					mpz_clears(base, even_n, r, temp, a, y, base_2, NULL);
					return false;
				}
				j++;

			}
			if (mpz_cmp(y, even_n) != 0) {
				mpz_clears(base, even_n, r, temp, a, y, base_2, NULL);
				return false;
			}
		}
	}
	mpz_clears(base, even_n, r, temp, a, y, base_2, NULL);
	return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	mpz_t rand;
	mpz_init(rand);
	do {
		mpz_urandomb(rand, state, bits);
		uint64_t y = mpz_sizeinbase(rand, 2);
		if (y < bits) {
			mpz_mul_2exp(rand, rand, (bits - y));
		}
		mpz_setbit(rand, 0);
	} while(!is_prime(rand, iters));
	mpz_set(p, rand);
	mpz_clear(rand);
}
