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
        printf("WARNING: bad subtract\n");
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
            subtract(&a, a, divisor);
            // printf("subtracted divisor: %llu %llu\n", divisor.hi, divisor.lo);
        }
        uint64_t carryOver = (divisor.hi & 0x01) << (BITSHIFT_64 - 1);
        divisor.hi = divisor.hi >> 1;
        divisor.lo = divisor.lo >> 1 + carryOver;

        if (divisor.hi == 0 && divisor.lo == 0){
            break;
        }
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
    uint128 modulus, 
    uint64_t pubExp,
    uint64_t msgLen){
    printf("encrypting\n");

    // if (msgLen * 8 < 128){
    //     char *msgEnd = (char *) plaintext;
    //     memset(&msgEnd[msgLen], 0, 128 - (msgLen * 8));
    //     plaintext->lo = plaintext->hi;
    //     plaintext->hi = 0;
    // }

    ciphertext->hi = plaintext->hi;
    ciphertext->lo = plaintext->lo;
    printf("plaintext: %llx %llx\n", ciphertext->hi, ciphertext->lo);

    while (pubExp > 0){
        // if (ciphertext->hi == 0 && ciphertext->lo == 0){
        //     printf("failed to encrypt, at pubExp: %llu\n", pubExp);
        //     break;
        // }
        multiply(ciphertext, *ciphertext, *plaintext);
        pubExp--;
        modulo(ciphertext, *ciphertext, modulus);
    }
    // printf("ciphertext: %llx %llx\n", ciphertext->hi, ciphertext->lo);
}

void rsa_decrypt(
    uint128 *decrypted, 
    uint128 *ciphertext, 
    uint128 modulus, 
    uint128 privateExp,
    uint64_t msgLen){
    printf("decrypting\n");
        
    decrypted->hi = ciphertext->hi;
    decrypted->lo = ciphertext->lo;
    uint128 decrement = {0, 1};

    while (privateExp.hi != 0 && privateExp.lo != 0){
        if (decrypted->hi == 0 && decrypted->lo == 0){
            printf("failed to decrypt \n");
            break;
        }
        multiply(decrypted, *decrypted, *ciphertext);
        subtract(&privateExp, privateExp, decrement);
        modulo(decrypted, *decrypted, modulus);
    }

    printf("decrypted: %llx %llx\n", decrypted->hi, decrypted->lo);
}
