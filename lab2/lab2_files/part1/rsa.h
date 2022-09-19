#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef struct uint128 {
    uint64_t hi;
    uint64_t lo;
} uint128;

typedef struct uint256 {
    uint128 hi128;
    uint128 lo128;
} uint256;

int compare128(uint128 a, uint128 b);
int compare256(uint256 a, uint256 b);

uint256 * createUint256();

int convert128To256(uint256 *result, uint128 val);

int add(uint256 *sum, uint256 a, uint256 b);
int subtract(uint256 *diff, uint256 a, uint256 b);

int multiply(uint256 *product, uint128 a, uint128 b);
int modulo(uint128 *rem, uint256 a, uint128 b);

void rsa_encrypt(
    uint128 *ciphertext, 
    const char *plaintext, 
    uint128 modulus, 
    uint64_t pubExp,
    uint64_t msgLen);

void rsa_decrypt(
    uint128 *decrypted, 
    uint128 *ciphertext, 
    uint128 modulus, 
    uint128 privateExp,
    uint64_t msgLen);
