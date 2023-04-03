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

void print_help() {
	fprintf(stderr, "Usage: ./decrypt [options]\n");
	fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
	fprintf(stderr, "  writing the result to the specified output file.\n");
	fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
	fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
	fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
	fprintf(stderr, "    -v          : Enable verbose output.\n");
	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
}

int main(int argc, char **argv) {
	char *in_file_name;
	char *out_file_name;
	bool user_in_file, user_out_file = false;
	char *pv_file_name = "rsa.priv";
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
				pv_file_name = optarg;
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
	mpz_t n, d;
	mpz_inits(n, d, NULL);
	FILE *pvfile;
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
	pvfile = fopen(pv_file_name, "r");
	if (pvfile == NULL) {
		printf("Could not open key file.\n");
		exit(1);
	}
	if (infile == NULL || outfile == NULL) {
		printf("Could not open in file or out file.\n");
		exit(1);
	}
	rsa_read_priv(n, d, pvfile);

	if (verbose) {
		gmp_printf("n - modulus (%d bits): %Zu\n", mpz_sizeinbase(n, 2), n);
		gmp_printf("d - private exponent (%d bits): %Zu\n", mpz_sizeinbase(d, 2), d);
	}

	rsa_decrypt_file(infile, outfile, n, d);



	mpz_clears(n, d, NULL);
	fclose(infile);
	fclose(outfile);
	fclose(pvfile);
}
