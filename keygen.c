#include <stdint.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#define OPTIONS "b:i:n:d:s:vh"

void print_help() {
	fprintf(stderr, "Usage: ./keygen [options]\n");
	fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
	fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
	fprintf(stderr, "  the program options.\n");
	fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
	fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
	fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
	fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
	fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
	fprintf(stderr, "    -v          : Enable verbose output.\n");
	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
}

int main(int argc, char **argv) {
	uint64_t seed = time(NULL);
	uint64_t nbits = 1024;
	uint64_t iters = 50;
	char *pb_name = "rsa.pub";
	char *pv_name = "rsa.priv";
	bool verbose = false;

	int opt; 

	while ((opt = getopt(argc, argv, OPTIONS)) != -1) { 
		switch (opt) {
			case 'b':
				nbits = atoi(optarg);
				if (nbits < 50 || nbits > 4096) {
					print_help();
					fprintf(stderr, "Bits must be in the range [50, 4096], inclusive.\n");
					exit(1);
				}
				break;
			case 'i':
				iters = atoi(optarg);
				if (iters < 1 || iters > 500) {
					print_help();
					fprintf(stderr, "Iterations must be in the range [1, 500], inclusive");
				}
				break;
			case 'n':
				pb_name = optarg;
				break;
			case 'd':
				pv_name = optarg;
				break;
			case 's':
				seed = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
				print_help();
				return 0;
			default:
				print_help();
				exit(1);
		}
	}
	randstate_init(seed);
	mpz_t p, q, n, e, s, d, m;
	mpz_inits(p, q, n, e, s, d, m, NULL);
	FILE *pbfile;
	FILE *pvfile;
	pbfile = fopen(pb_name, "w");
	pvfile = fopen(pv_name, "w");
	if (pbfile == NULL || pvfile == NULL) {
		fprintf(stderr, "Could not private key or public keyfiles.\n");
		exit(1);
	}
	int num = fileno(pvfile);
	if (fchmod(num, 0600) != 0) {
		fprintf(stderr, "Could not set read and write privileges for private key.\n");
	}
	rsa_make_pub(p, q, n, e, nbits, iters);
	rsa_make_priv(d, e, p, q);
	char *user = getenv("USER");
	mpz_set_str(m, user, 62);
	rsa_sign(s, m, d, n);
	rsa_write_pub(n, e, s, user, pbfile);
	rsa_write_priv(n, d, pvfile);
	fclose(pbfile);
	fclose(pvfile);

	if (verbose) {
		printf("username: %s\n", user);
		gmp_printf("user signature (%d bits): %Zu\n", mpz_sizeinbase(s, 2), s);
		gmp_printf("p (%d bits): %Zu\n", mpz_sizeinbase(p, 2), p);
		gmp_printf("q (%d bits): %Zu\n", mpz_sizeinbase(q, 2), q);
		gmp_printf("n(%d bits): %Zu\n", mpz_sizeinbase(n, 2), n);
		gmp_printf("e (%d bits): %Zu\n", mpz_sizeinbase(e, 2), e);
		gmp_printf("d (%d bits): %Zu\n", mpz_sizeinbase(d, 2), d);
	}
	randstate_clear();
	mpz_clears(p, q, n, e, s, d, m, NULL);
}
