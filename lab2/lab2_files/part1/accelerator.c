#include "rsa.h"

#define BITSHIFT_32 32
#define HIGH_MASK_32 0xFFFF0000
#define LOW_MASK_32 0x0000FFFF

#define CARRY_OVER_BIT 0x8000000000000000
#define BITSHIFT_64 64

void add(uint128 *sum, uint128 a, uint128 b){
    sum->lo = a.lo + b.lo;
    uint64_t overflow = sum->lo < a.lo && sum->lo < b.lo ? 1 : 0;
    sum->hi = a.hi + b.hi + overflow;
}

void subtract(uint128 *diff, uint128 a, uint128 b){
    if (a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo)){
        diff->hi = 0;
        diff->lo = 0;
        return;
    }

    if (a.lo < b.lo){
        uint64_t borrow = 0xffffffffffffffffLL;
        a.hi -= 1;
        borrow -= b.lo;
        a.lo += (borrow + 1);
        diff->lo = a.lo;
        diff->hi = a.hi - b.hi;
        return;
    }

    diff->lo = a.lo - b.lo;
    diff->hi = a.hi - b.hi;
}

void multiply(uint128 *product, uint128 a, uint128 b){
    product->hi = 0;
    product->lo = 0;

    uint128 sum, *smaller;
    if (a.hi > b.hi || (a.hi == b.hi && a.lo > b.lo)){
        sum = a;
        smaller = &b;
    }
    else {
        sum = b;
        smaller = &a;
    }

    uint64_t remainder = smaller->hi;
    uint64_t val = smaller->lo;

    // printf("sum: %llx %llx\n", sum.hi, sum.lo);
    // printf("remainder: %llu, val: %llu\n", remainder, val);

    while (!(val == 0 && remainder == 0)){
        if ((val % 2) == 1){
            add(product, sum, *product);
        }

        // double the larger number
        uint64_t carryOver = (sum.lo & CARRY_OVER_BIT) >> (BITSHIFT_64 - 1);
        sum.lo = sum.lo << 1;
        sum.hi = (sum.hi << 1) + carryOver;

        // divide the smaller number
        val = val >> 1;
        val += (remainder % 2) << (BITSHIFT_64 - 1);
        remainder = remainder >> 1;
    }
}

void modulo(uint128 *rem, uint128 a, uint128 b){
    uint128 divisor = b;

    while ((divisor.hi <= a.hi / 2 && divisor.lo < a.lo / 2)){
        uint64_t carryOver = (divisor.lo & CARRY_OVER_BIT) >> (BITSHIFT_64 - 1);
        divisor.lo = divisor.lo << 1;
        divisor.hi = (divisor.hi << 1) + carryOver;
    }

    while (a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo)){
        if (a.hi > divisor.hi || (a.hi == divisor.hi && a.lo >= divisor.lo)){
            subtract(&a, a, b);
        }
        uint64_t carryOver = (divisor.hi & 0x01) << (BITSHIFT_64 - 1);
        divisor.hi = divisor.hi >> 1;
        divisor.lo = divisor.lo >> 1 + carryOver;
    }

    rem->hi = a.hi;
    rem->lo = a.lo;
}

/**
 * modulus = prime1 * prime2
 * e = pubExp
 * d = privateExp
 */
void rsa_encrypt(
    uint128 *ciphertext, 
    uint128 *plaintext, 
    uint64_t msgLen, 
    uint128 *modulus, 
    uint64_t pubExp){
    
    // Clean up message if needed
    if (msgLen < sizeof(uint64_t) * 2){
        char *temp = (char *)plaintext;
        memset(&temp[msgLen], 0, 128 - msgLen);
    }
    
    // while (pubExp > 0){
    //     multiply(ciphertext, *ciphertext, *plaintext);
    //     pubExp--;
    //     modulo(ciphertext, *ciphertext, *modulus);
    // }

}

void rsa_decrypt(
    uint128 *decrypted, 
    uint128 *ciphertext, 
    uint128 *modulus, 
    uint128 *privateExp){
}
