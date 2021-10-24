#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <gmpxx.h>
using namespace std;

#define K 100
#define PADDING true
#define AUTO true

void generatePrime(mpz_t &randomPrime, gmp_randstate_t state) {
    cout << "\n Generating new Prime";
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
    cout << "\n Checking if generated number is prime";
    // Check if the resulting number is prime
    int result = mpz_probab_prime_p(randomPrime, 15);
    printf("\n Result: %d", result);
    if (result == 2 || result == 1) {
        return;
    } else {
        generatePrime(randomPrime, state);
    }
}

void RSAkeygen() {
    // Initialize p, q, n, e, and d
    mpz_t intP, intQ, intN, intE, intD;
    mpz_init(intP);
    mpz_init(intQ);
    mpz_init(intN);
    mpz_init(intE);
    mpz_init(intD);
    cout << "Generating p and q";
    int result = 0;
    // Generate p, q and n
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    while (result == 0) {
        generatePrime(intP, state);
        generatePrime(intQ, state);
        result = mpz_cmp(intP, intQ);
        char* pVal = new char[50000];
        char* qVal = new char[50000];
        printf("\n P Val: ");
        mpz_get_str(pVal, 10, intP);
        cout << pVal;
        printf("\n Q Val: ");
        mpz_get_str(qVal, 10, intQ);
        cout << qVal;
    }
    mpz_mul(intN, intP, intQ);
    cout << "\n Determining Phi";
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
    cout << "\n Generating e";
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

void RSAencrypt(mpz_t &message, mpz_t &e, mpz_t &n, char* location) {
    mpz_t result;
    mpz_init(result);
    mpz_powm(result, message, e, n);
//    char* resultString = new char[50000];
//    char* messageString = new char[50000];
//    printf("\n Encryption Result Val: ");
//    mpz_get_str(resultString, 10, result);
//    cout << resultString;
//    printf("\n Message Val: ");
//    mpz_get_str(messageString, 10, message);
//    cout << messageString;
    FILE* found = fopen(location, "w");
    mpz_out_str(found, 10, result);
    fclose(found);
}

void RSAdecrypt(mpz_t &message, mpz_t &d, mpz_t &n, char* location) {
    mpz_t result;
    mpz_init(result);
    mpz_powm(result, message, d, n);
    
//    char* resultString = new char[50000];
//    char* messageString = new char[50000];
//    printf("\n Decryption Result Val: ");
//    mpz_get_str(resultString, 10, result);
//    cout << resultString << "\n";
//    printf("\n Message Val: ");
//    mpz_get_str(messageString, 10, message);
//    cout << messageString << "\n";

    FILE* found = fopen(location, "w");
    mpz_out_str(found, 10, result);
    fclose(found);
}

void RSAPaddingEncrypt(mpz_t &message, size_t &messageBytes, mpz_t &e, mpz_t &n, char* location) {
    mpz_t paddedMessage;
    mpz_init(paddedMessage);
    
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    size_t maxBytes = K / 8;
    size_t randomBytes = maxBytes - messageBytes - 3;
    mpz_t random;
    mpz_init(random);
    cout << "Generating random number of byte size: " << randomBytes << "\n";
    mpz_urandomb(random, state, randomBytes * 8);
    // TODO: Step through each of these and find where things go wrong
//    FILE* testing;
//    testing = fopen("program_files/testing.txt", "w");
//    mpz_out_str(testing, paddedMessage);
    
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
    mpz_mul_2exp(paddedMessage, paddedMessage, (messageBytes * 8));
    mpz_ior(paddedMessage, paddedMessage, message);
    
    mpz_set(message, paddedMessage);
    RSAencrypt(message, e, n, location);
}

void RSAPaddingDecrypt(mpz_t &message, size_t &messageBytes, mpz_t &d, mpz_t &n, char* location) {
    size_t paddedMessageBytes = mpz_sizeinbase(message, 2) / 8;
    mpz_t retreiveMessage;
    mpz_init2(retreiveMessage, paddedMessageBytes * 8);
    for (int i = 0; i < (messageBytes * 8); i++) {
        mpz_setbit(retreiveMessage, i);
    }
    mpz_and(message, message, retreiveMessage);
    RSAdecrypt(message, d, n, location);
}

int main(int argc, char** argv ) {
    srand(time(NULL));
    cout << "Generating new keys";
    RSAkeygen();
    
    mpz_t p, q, n, e, d, message, endmessage;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);
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
    FILE *dfileout, *dfile;
    dfile = fopen("program_files/d.txt", "r");
    mpz_inp_str(d, dfile, 10);
    if (!AUTO) {
        dfileout = fopen(file_location, "w");
    } else {
        dfileout = fopen("program_files/d.txt", "w");
    }
    mpz_out_str(dfileout, 10, d);
    
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
    mpz_mul(n, p, q);
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
    
    
    size_t messageBytes = mpz_sizeinbase(message, 2) / 8;
    
    if (PADDING) {
        RSAPaddingEncrypt(message, messageBytes, e, n, encryption);
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
        RSAPaddingDecrypt(endmessage, messageBytes, d, n, decryption);
    } else {
        RSAdecrypt(endmessage, d, n, decryption);
    }
    
    fclose(pfile);
    fclose(qfile);
    fclose(efile);
    fclose(dfile);
    fclose(dfileout);
    fclose(nfile);
    fclose(cfile);
    fclose(xfile);
}
