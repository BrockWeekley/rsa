#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <gmpxx.h>
using namespace std;

// Define the size of N in bits
#define K 2048

// Set the program to use Message Padding 1.5
#define PADDING true

// Automatically enter the locations of program generated files
#define AUTO false

// Generate a random prime value of K/2 size and check that it is prime before returning
void generatePrime(mpz_t &randomPrime, gmp_randstate_t state) {
    // Seed and Generate a random number of size bits / 2
    mpz_urandomb(randomPrime, state, K/2);
    // Set the lowest bit of the generated number
    mpz_t one;
    mpz_init(one);
    mpz_set_ui(one, 1);
    mpz_ior(randomPrime, randomPrime, one);
    // Set the two highest bits of the number
    mp_bitcnt_t highestBit = mp_bitcnt_t(K/2);
    mp_bitcnt_t secondHighestBit = mp_bitcnt_t(K/2 - 1);
    mpz_setbit(randomPrime, highestBit);
    mpz_setbit(randomPrime, secondHighestBit);
    // Check if the resulting number is prime
    int result = mpz_probab_prime_p(randomPrime, 15);
    if (result == 2 || result == 1) {
        return;
    } else {
        generatePrime(randomPrime, state);
    }
}

// Generate the RSA keys
void RSAkeygen() {
    // Initialize p, q, n, e, and d
    mpz_t intP, intQ, intN, intE, intD;
    mpz_init(intP);
    mpz_init(intQ);
    mpz_init(intN);
    mpz_init(intE);
    mpz_init(intD);
    int result = 0;
    // Generate p, q and n
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    while (result == 0) {
        generatePrime(intP, state);
        generatePrime(intQ, state);
        result = mpz_cmp(intP, intQ);
    }
    mpz_mul(intN, intP, intQ);
    // Initialize phi values
    mpz_t phiN, phiP, phiQ, gcdNE;
    mpz_init(phiN);
    mpz_init(gcdNE);
    mpz_init(phiP);
    mpz_init(phiQ);
    // Determine phi(n)
    mpz_sub_ui(phiP, intP, 1);
    mpz_sub_ui(phiQ, intQ, 1);
    mpz_mul(phiN, phiP, phiQ);
    // Generate e
    mpz_urandomm(intE, state, phiN);
    mpz_gcd(gcdNE,intE, phiN);
    while (mpz_cmp_ui(gcdNE, 1) != 0) {
        mpz_urandomm(intE, state, phiN);
        mpz_gcd(gcdNE,intE, phiN);
    }
    // Generate d
    mpz_invert(intD, intE, phiN);
    // Write values to files
    FILE *pfile, *qfile, *efile, *dfile;
    pfile = fopen("program_files/p.txt", "w");
    qfile = fopen("program_files/q.txt", "w");
    efile = fopen("program_files/e.txt", "w");
    dfile = fopen("program_files/d.txt", "w");
    mpz_out_str(pfile, 10, intP);
    mpz_out_str(qfile, 10, intQ);
    mpz_out_str(efile, 10, intE);
    mpz_out_str(dfile, 10, intD);
    fclose(pfile);
    fclose(qfile);
    fclose(efile);
    fclose(dfile);
}

// Apply the RSA encryption algorithm with the provided values
void RSAencrypt(mpz_t &message, mpz_t &e, mpz_t &n, char* location) {
    mpz_t result;
    mpz_init(result);
    mpz_powm(result, message, e, n);

    FILE* found = fopen(location, "w");
    mpz_out_str(found, 10, result);
    fclose(found);
}

// Apply the RSA decryption algorithm with the provided values
void RSAdecrypt(mpz_t &message, mpz_t &d, mpz_t &n, char* location, bool output) {
    mpz_t result;
    mpz_init(result);
    mpz_powm(result, message, d, n);
    
    if (output) {
        FILE* found = fopen(location, "w");
        mpz_out_str(found, 10, result);
        fclose(found);
    } else {
        mpz_set(message, result);
    }
}

// Apply the RSA message padding 1.5 encryption algorithm with the provided values
void RSAPaddingEncrypt(mpz_t &message, size_t &messageBits, mpz_t &e, mpz_t &n, char* location) {
    mpz_t paddedMessage;
    mpz_init(paddedMessage);
    
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    size_t messageBytes = (messageBits + (8 - 1)) / 8;
    size_t maxBytes = (K + (8 - 1)) / 8;
    size_t randomBytes = maxBytes - messageBytes - 3;
    mpz_t random;
    mpz_init(random);
    mpz_urandomb(random, state, randomBytes * 8);
    
    // Left shift 8 bits for zero
    mpz_mul_2exp(paddedMessage, paddedMessage, 8);
    // Left shift 8 bits for two (bt)
    mpz_mul_2exp(paddedMessage, paddedMessage, 8);
    mpz_setbit(paddedMessage, 1);
    // Left shift size of random message in bits
    mpz_mul_2exp(paddedMessage, paddedMessage, (randomBytes * 8));
    mpz_ior(paddedMessage, paddedMessage, random);
    // Left shift 8 bits for zero
    mpz_mul_2exp(paddedMessage, paddedMessage, 8);
    // Left shift size of input message in bits
    mpz_mul_2exp(paddedMessage, paddedMessage, messageBits);
    mpz_ior(paddedMessage, paddedMessage, message);
    mpz_set(message, paddedMessage);
    RSAencrypt(message, e, n, location);
}

// Apply the RSA message padding 1.5 encryption algorithm with the provided values
void RSAPaddingDecrypt(mpz_t &message, size_t &messageBits, mpz_t &d, mpz_t &n, char* location) {
    
    RSAdecrypt(message, d, n, location, false);
    
    size_t paddedMessageBytes = mpz_sizeinbase(message, 2) / 8;
    mpz_t retreiveMessage;
    mpz_init2(retreiveMessage, paddedMessageBytes * 8);
    for (int i = 0; i < messageBits; i++) {
        mpz_setbit(retreiveMessage, i);
    }
    
    mpz_and(message, message, retreiveMessage);
    
    FILE* found = fopen(location, "w");
    mpz_out_str(found, 10, message);
    fclose(found);
}

int main(int argc, char** argv ) {
    srand(time(NULL));
    
    if (AUTO) {
        cout << "Generating new keys \n";
        RSAkeygen();
    }

    mpz_t p, q, n, e, d, phiP, phiQ, phiN, message, endmessage;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);
    mpz_init(phiP);
    mpz_init(phiQ);
    mpz_init(phiN);
    mpz_init(message);
    mpz_init(endmessage);
    
    char *file_location = new char[100];
    if (!AUTO) {
        cout << "\n Enter the name of the file that contains p: \n";
        cin >> file_location;
    }
    FILE *pfile;
    if (!AUTO) {
        pfile = fopen(file_location, "r");
    } else {
        pfile = fopen("program_files/p.txt", "r");
    }
    mpz_inp_str(p, pfile, 10);
    
    if (!AUTO) {
        cout << "Enter the name of the file that contains q: \n";
        cin >> file_location;
    }
    FILE *qfile;
    if (!AUTO) {
        qfile = fopen(file_location, "r");
    } else {
        qfile = fopen("program_files/q.txt", "r");
    }
    mpz_inp_str(q, qfile, 10);

    if (!AUTO) {
        cout << "Enter the name of the file that contains e: \n";
        cin >> file_location;
    }
    FILE *efile;
    if (!AUTO) {
        efile = fopen(file_location, "r");
    } else {
        efile = fopen("program_files/e.txt", "r");
    }
    mpz_inp_str(e, efile, 10);

    if (!AUTO) {
        cout << "Enter the output file name to store d: \n";
        cin >> file_location;
    }
    
    FILE *dfile;
    
    if (!AUTO) {
        dfile = fopen(file_location, "w");
    } else {
        dfile = fopen("program_files/d.txt", "r");
        mpz_inp_str(d, dfile, 10);
    }
    
    if (!AUTO) {
        cout << "Enter the output file name to store N: \n";
        cin >> file_location;
    }
    FILE *nfile;
    if (!AUTO) {
        nfile = fopen(file_location, "w");
    } else {
        nfile = fopen("program_files/N.txt", "w");
    }
    
    // Compute n
    mpz_mul(n, p, q);
    
    if (!AUTO) {
        // Compute d
        mpz_sub_ui(phiP, p, 1);
        mpz_sub_ui(phiQ, q, 1);
        mpz_mul(phiN, phiP, phiQ);
        mpz_invert(d, e, phiN);
    }
    
    mpz_out_str(dfile, 10, d);
    
    mpz_out_str(nfile, 10, n);
    
    if (!AUTO) {
        cout << "Enter the name of the file that contains x to be encrypted using (N, e): \n";
        cin >> file_location;
    }
    FILE *xfile;
    if (!AUTO) {
        xfile = fopen(file_location, "r");
    } else {
        xfile = fopen("program_files/x.txt", "r");
    }
    mpz_inp_str(message, xfile, 10);
    
    cout << "Enter the output file name to store E(x): \n";
    cin >> file_location;
    char* encryption = new char[100];
    encryption = file_location;
    
    
    size_t messageBits = mpz_sizeinbase(message, 2);
    
    if (PADDING) {
        RSAPaddingEncrypt(message, messageBits, e, n, encryption);
    } else {
        RSAencrypt(message, e, n, encryption);
    }

    if (!AUTO) {
        cout << "Enter the name of the file that contains c to be decrypted using d: \n";
        cin >> file_location;
    }
    FILE *cfile;
    if (!AUTO) {
        cfile = fopen(file_location, "r");
    } else {
        cfile = fopen("program_files/encryption.txt", "r");
    }
    mpz_inp_str(endmessage, cfile, 10);
    
    cout << "Enter the output file name to store D(c): \n";
    cin >> file_location;
    char* decryption = new char[100];
    decryption = file_location;
    
    if (PADDING) {
        RSAPaddingDecrypt(endmessage, messageBits, d, n, decryption);
    } else {
        RSAdecrypt(endmessage, d, n, decryption, true);
    }
    
    fclose(pfile);
    fclose(qfile);
    fclose(efile);
    fclose(dfile);
    fclose(nfile);
    fclose(cfile);
    fclose(xfile);
}
