#include <stdint.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#define OPTIONS "i:o:n:vh"
#define MAXLINE 1000

void print_help() {
	fprintf(stderr, "Usage: ./encrypt [options]\n");
	fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
	fprintf(stderr, "  writing the result to the specified output file.\n");
	fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
	fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
	fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
	fprintf(stderr, "    -v          : Enable verbose output.\n");
	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
}

int main(int argc, char **argv) {
	char *in_file_name;
	char *out_file_name;
	bool user_in_file, user_out_file = false;
	char *pk_file_name = "rsa.pub";
	bool verbose = false;
	int opt;

	while ((opt = getopt(argc, argv, OPTIONS)) != -1) { 
		switch (opt) {
			case 'i':
				in_file_name = optarg;
				user_in_file = true;
				break;
			case 'o':
				out_file_name = optarg;
				user_out_file = true;
				break;
			case 'n':
				pk_file_name = optarg;
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
	mpz_t n, e, s, m;
	char username[MAXLINE];
	mpz_inits(n, e, s, m, NULL);
	FILE *pbfile;
	FILE *infile;
	FILE *outfile;
	if (user_in_file == true) {
		infile = fopen(in_file_name, "r");
	}
	else {
		infile = stdin;
	}
	if (user_out_file == true) {
		outfile = fopen(out_file_name, "w");
	}
	else {
		outfile = stdout;
	}
	pbfile = fopen(pk_file_name, "r");
	if (pbfile == NULL) {
		printf("Could not open key file.\n");
		exit(1);
	}
	if (infile == NULL || outfile == NULL) {
		printf("Could not open in file or out file.\n");
		exit(1);
	}
	rsa_read_pub(n, e, s, username, pbfile);
	mpz_set_str(m, username, 62);
	if (verbose) {
		printf("username: %s\n", username);
		gmp_printf("user signature (%d bits): %Zu\n", mpz_sizeinbase(s, 2), s);
		gmp_printf("n - modulus (%d bits): %Zu\n", mpz_sizeinbase(n, 2), n);
		gmp_printf("e - public exponent (%d bits): %Zu\n", mpz_sizeinbase(e, 2), e);
	}
	if (rsa_verify(m, s, e, n) == true) {
		rsa_encrypt_file(infile, outfile, n, e);
	}
	mpz_clears(n, e, s, m, NULL);
	fclose(infile);
	fclose(outfile);
	fclose(pbfile);
}
