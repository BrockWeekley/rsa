#include <stdio.h>
#include <iostream>
#include <string>
#include <gmpxx.h>
#include <filesystem>
#include <unistd.h>
using namespace std;

#define K 2048
#define PADDING false

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
    // Initialize p, q, n, and e
    mpz_t intP, intQ, intN, intE;
    mpz_init2(intP, K/2);
    mpz_init2(intQ, K/2);
    mpz_init2(intN, K);
    mpz_init(intE);
    cout << "Generating p and q";
    int result = 0;
    // Generate p, q and n
    gmp_randstate_t state;
    gmp_randinit_default(state);
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
    mpz_t phiN, phiP, phiQ, gcdNE, nMinusOne;
    mpz_init(phiN);
    mpz_init(gcdNE);
    mpz_init(nMinusOne);
    mpz_init2(phiP, K/2);
    mpz_init2(phiQ, K/2);
    // Determine phi(n)
    mpz_sub_ui(phiP, intP, 1);
    mpz_sub_ui(phiQ, intQ, 1);
    mpz_mul(phiN, phiP, phiQ);
    mpz_sub_ui(nMinusOne, phiN, 1);
    cout << "\n Generating e";
    // Generate e
    mpz_random(intE, mpz_size(nMinusOne));
    mpz_gcd(gcdNE,intE, phiN);
    while (mpz_cmp_ui(gcdNE, 1) != 0) {
        mpz_random(intE, mpz_size(nMinusOne));
        mpz_gcd(gcdNE,intE, phiN);
    }
    cout << "\n Writing files";
    char tmp[256];
    getcwd(tmp, 256);
    cout << "\n Working: " << tmp << endl;
    // Write values to files
    FILE *pfile, *qfile, *efile;
    pfile = fopen("program_files/p.txt", "ab+");
    qfile = fopen("program_files/q.txt", "ab+");
    efile = fopen("program_files/e.txt", "ab+");
    size_t size = mpz_out_str(pfile, 10, intP);
    printf("\n Size of pfile: %zu", size);
    size = mpz_out_str(qfile, 10, intQ);
    printf("\n Size of qfile: %zu", size);
    size = mpz_out_str(efile, 10, intE);
    printf("\n Size of efile: %zu", size);
    fclose(pfile);
    fclose(qfile);
    fclose(efile);
}

void RSAencrypt() {
    
}

void RSAdecrypt() {
    
}

void RSAPaddingEncrypt() {
    
}

void RSAPaddingDecrypt() {
    
}

int main(int argc, char** argv ) {
    cout << "Generating new keys";
    RSAkeygen();
    
    mpz_t p, q, n, e, d;
    mpz_init2(p, K / 2);
    mpz_init2(q, K / 2);
    mpz_init2(n, K);
    mpz_init(e);
    mpz_init(d);
    
    char *file_location = new char[100];
    cout << "\n Enter the name of the file that contains p: \n";
    cin >> file_location;
    FILE *pfile;
    pfile = fopen(file_location, "r");
    mpz_inp_str(p, pfile, 10);
    
    cout << "Enter the name of the file that contains q: \n";
    cin >> file_location;
    FILE *qfile;
    qfile = fopen(file_location, "r");
    mpz_inp_str(q, qfile, 10);

    
    cout << "Enter the name of the file that contains e: \n";
    cin >> file_location;
    FILE *efile;
    efile = fopen(file_location, "r");
    mpz_inp_str(e, efile, 10);

    cout << "Enter the output file name to store d: \n";
    cin >> file_location;
    FILE *dfile;
    dfile = fopen(file_location, "ab+");
    mpz_out_str(dfile, 10, d);
    
    cout << "Enter the output file name to store N: \n";
    cin >> file_location;
    FILE *nfile;
    nfile = fopen(file_location, "ab+");
    
    cout << "Enter the name of the file that contains x to be encrypted using (N, e): \n";
    cin >> file_location;
    FILE *xfile;
    xfile = fopen(file_location, "r");
//
//    cout << "Enter the output file name to store E(x): \n";
//    string encryptionout;
//    cin >> encryptionout;
//
//    cout << "Enter the name of the file that contains c to be decrypted using d: \n";
//    string cfile;
//    cin >> cfile;
//
//    cout << "Enter the output file name to store D(c): \n";
//    string decryptionout;
//    cin >> decryptionout;

    fclose(pfile);
    fclose(qfile);
    fclose(efile);
    fclose(dfile);
    fclose(nfile);
}
