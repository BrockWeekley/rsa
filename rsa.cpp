#include <stdio.h>
#include <iostream>
#include <string>
#include </usr/local/include/gmp.h>
#include </usr/local/include/gmpxx.h>
using namespace std;

#define K 10
#define PADDING false

void generatePrime(mpz_t &randomPrime) {
    // Seed and Generate a random number of size bits / 2
    gmp_randstate_t state;
    gmp_randinit_default(state);
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
    if (result == 2) {
        return;
    } else {
        generatePrime(randomPrime);
    }
}

void RSAkeygen() {
    // Initialize p, q, n, and e
    mpz_t intP, intQ, intN, intE;
    mpz_init2(intP, K/2);
    mpz_init2(intQ, K/2);
    mpz_init2(intN, K);
    mpz_init(intE);
    // Generate p, q and n
    while (mpz_cmp(intP, intQ) == 0) {
        generatePrime(intP);
        generatePrime(intQ);
    }
    mpz_mul(intN, intP, intQ);
    // Initialize phi values
    mpz_t phiN, phiP, phiQ, gcdNE;
    mpz_init(phiN);
    mpz_init(gcdNE);
    mpz_init2(phiP, K/2);
    mpz_init2(phiQ, K/2);
    // Determine phi(n)
    mpz_sub_ui(phiP, intP, 1);
    mpz_sub_ui(phiQ, intQ, 1);
    mpz_mul(phiN, phiP, phiQ);
    // Generate e
    // TODO: I don't think this line will work:
    mpz_random(intE, mp_size_t(phiN - 1));
    mpz_gcd(gcdNE,intE, phiN);
    while (mpz_cmp_ui(gcdNE, 1) != 0) {
        mpz_random(intE, mp_size_t(phiN - 1));
        mpz_gcd(gcdNE,intE, phiN);
    }
}

int main(int argc, char** argv ) {
    RSAkeygen();
    
    mpz_t p, q, e;
    
    char *file_location = new char[100];
    cout << "Enter the name of the file that contains p: \n";
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


    cout << "Enter the output file name to store d and N: \n";
    string dNout;
    cin >> dNout;
    
    cout << "Enter the name of the file that contains x to be encrypted using (N, e): \n";
    string xfile;
    cin >> xfile;
    
    cout << "Enter the output file name to store E(x): \n";
    string encryptionout;
    cin >> encryptionout;
    
    cout << "Enter the name of the file that contains c to be decrypted using d: \n";
    string cfile;
    cin >> cfile;
    
    cout << "Enter the output file name to store D(c): \n";
    string decryptionout;
    cin >> decryptionout;

}
