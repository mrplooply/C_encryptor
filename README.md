# RSA Cryptography

## Introduction

The three main files, keygen.c, encrypt.c, and decrypt.c are use togther in order to encrypt and decrypt files using the RSA algorithm. In order to use the program the user must complile all files including the 3 libraries, rsa.c, numtheory.c, and randstate.c. Then create a public key by running keygen.c, encypt a file with encrypt.c and decrypt a file with decrypt.c. 

## Building
To build the executable file use the command

> make

or

> make all

## Running

### Key Generation

To run keygen you may or may not provide an argument.

To set the minimum bits needed for the public modulus n (default: 1024).

> ./keygen -b [argument]

To set the number of Miller-Rabin iterations for testing primes (default: 50).

> ./keygen -i [arugment]

To choose the public key file (default: rsa.pub).

> ./keygen -n [filename]

To choose the private key file (default: rsa.priv).

> ./keygen -d [filename]

To choose the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL)).

> ./keygen -s [seed]

To enable verbose output.

> ./keygen -v

To display program synopsis and usage.

> ./keygen -h

### Encrypt File

To run encrypt you may or may not provide an argument.

To set the input file to encrypt (default: stdin).

> ./encrypt -i [filename]

To set the output file to encrypt (default: stdout).

> ./encrypt -o [filename]

To set the file containing the public key (default: rsa.pub).

> ./encrypt -n [filename]

To enable verbose output.

> ./encrypt -v

To display program synopsis and usage.

> ./decrypt -h

### Decrypt File

To run decrypt you may or may not provide an argument.

To set the input file to encrypt (default: stdin).

> ./decrypt -i [filename]

To set the output file to encrypt (default: stdout).

> ./decrypt -o [filename]

To set the file containing the private key (default: rsa.priv).

> ./decrypt -n [filename]

To enable verbose output.

> ./decrypt -v

To display program synopsis and usage.

> ./decrypt -h

## Contributing

rsa.c, numtheory.c, randinit.c, keygen.c, encrypt.c, decrypt.c Makefile, rsa.h, randinit.h, numtheory.h Nicholas Barsi-Rhyne 11/06/2022
