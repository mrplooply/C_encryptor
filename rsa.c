#include <unistd.h>
#include "rsa.h"
#include "randstate.h"
#include "numtheory.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	uint64_t rand = random();
	uint64_t p_bits = (rand % (nbits / 4)) + (nbits / 4);
	uint64_t q_bits = nbits - p_bits;

	make_prime(p, p_bits, iters);
	make_prime(q, q_bits, iters);
	mpz_mul(n, p, q);

	mpz_t p_sub_one, q_sub_one, gcd_temp, lcm_pq;
	mpz_init_set(p_sub_one, p);
	mpz_init_set(q_sub_one, q);
	mpz_init(gcd_temp);
	mpz_init(lcm_pq);
	mpz_sub_ui(p_sub_one, p, 1);
	mpz_sub_ui(q_sub_one, q, 1);

	gcd(gcd_temp, p_sub_one, q_sub_one);
	mpz_mul(lcm_pq, p_sub_one, q_sub_one);
	mpz_div(lcm_pq, lcm_pq, gcd_temp);
	mpz_lcm(lcm_pq, p_sub_one, q_sub_one);
	do {
		 mpz_urandomb(e, state, nbits);
		 gcd(gcd_temp, lcm_pq, e);
	} while(mpz_cmp_ui(gcd_temp, 1) != 0);
	mpz_clears(p_sub_one, q_sub_one, gcd_temp, lcm_pq, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	gmp_fprintf(pbfile, "%Zx\n", n);
	gmp_fprintf(pbfile, "%Zx\n", e);
	gmp_fprintf(pbfile, "%Zx\n", s);
	fprintf(pbfile, "%s", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	mpz_inp_str(n, pbfile, 16);
	mpz_inp_str(e, pbfile, 16);
	mpz_inp_str(s, pbfile, 16);
	//get last line of file
	while (fgets(username, 1000, pbfile) != NULL) {
		continue;
	}
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	mpz_t p_sub_one, q_sub_one, gcd_temp, lcm_pq;
	mpz_init_set(p_sub_one, p);
	mpz_init_set(q_sub_one, q);
	mpz_init(gcd_temp);
	mpz_init(lcm_pq);
	mpz_sub_ui(p_sub_one, p, 1);
	mpz_sub_ui(q_sub_one, q, 1);

	gcd(gcd_temp, p_sub_one, q_sub_one);
	mpz_mul(lcm_pq, p_sub_one, q_sub_one);
	mpz_div(lcm_pq, lcm_pq, gcd_temp);
	mod_inverse(d, e, lcm_pq);
	mpz_clears(p_sub_one, q_sub_one, gcd_temp, lcm_pq, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	gmp_fprintf(pvfile, "%Zx\n", n);
	gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	mpz_inp_str(n, pvfile, 16);
	mpz_inp_str(d, pvfile, 16);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
	pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	size_t k;
	k = mpz_sizeinbase(n, 2) / 8;
	uint8_t *buf;
	buf = (uint8_t *)calloc(k, sizeof(uint8_t));
	buf[0] = 0xFF;

	mpz_t m, c;
	mpz_inits(m, c, NULL);
	size_t bytes; 

	while ((bytes = fread((buf + 1), sizeof(uint8_t), k - 2, infile)) != 0) {
		mpz_import(m, bytes + 1, 1, sizeof(uint8_t),1 ,0 , buf);
		rsa_encrypt(c, m, e, n);
		gmp_fprintf(outfile, "%Zx\n", c);
	}
	mpz_clears(m, c, NULL);
	free(buf);
} 

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	uint64_t k;
	k = mpz_sizeinbase(n, 2) / 8;
	uint8_t *block;
	block = (uint8_t *)calloc(k, sizeof(uint8_t));
	mpz_t c, m;
	mpz_inits(c, m, NULL);
	while (mpz_inp_str(c, infile, 16) != 0) {
		size_t j;
		rsa_decrypt(m, c, d, n);
		mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
		for (uint64_t i = 1; i < j; i++) {
			unsigned char c = block[i];
			fputc(c, outfile);
		}
	}
	free(block);
	mpz_clears(c, m, NULL);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	pow_mod(s, m, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	mpz_t cmp_m;
	mpz_init(cmp_m);
	pow_mod(cmp_m, s, e, n);
	if (mpz_cmp(cmp_m, m) == 0) {
		mpz_clear(cmp_m);
		return true;
	}
	mpz_clear(cmp_m);
	return false;
}
