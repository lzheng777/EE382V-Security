#include "rsa.h"

#define CARRY_OVER_BIT 0x8000000000000000
#define BITSHIFT_64 64

int convert128To256(uint256 *result, uint128 val){
    if (result == NULL){
        return -1;
    }

    result->lo128 = val;
    result->hi128.hi = 0;
    result->hi128.lo = 0;
    return 0;
}

int compare128(uint128 a, uint128 b){
    if (a.hi == b.hi && a.lo == b.lo)
        return 0;
    
    if (a.hi > b.hi || (a.hi == b.hi && a.lo > b.lo))
        return 1;

    return -1;
}

int compare256(uint256 a, uint256 b){
    int hiCompare = compare128(a.hi128, b.hi128);
    int lowCompare = compare128(a.lo128, b.lo128);
    if (hiCompare == 0 && lowCompare == 0)
        return 0;
    
    if (a.hi128.hi == b.hi128.hi){
        if (a.hi128.lo == b.hi128.lo){
            if (a.lo128.hi == b.lo128.hi){
                if (a.lo128.lo == b.lo128.lo){
                    return 0;
                }
                else if (a.lo128.lo > b.lo128.lo){
                    return 1;
                }
            }
            else if (a.lo128.hi > b.lo128.hi){
                return 1;
            }
        }
        else if (a.hi128.lo > b.hi128.lo){
            return 1;
        }
    }
    else if (a.hi128.hi > b.hi128.hi){
        return 1;
    }

    return -1;
}

void double256(uint256 *val){
    uint8_t carryOver = (val->lo128.lo & CARRY_OVER_BIT) >> (BITSHIFT_64 - 1);
    uint8_t carryOver2 = (val->lo128.hi & CARRY_OVER_BIT) >> (BITSHIFT_64 - 1);
    uint8_t carryOver3 = (val->hi128.lo & CARRY_OVER_BIT) >> (BITSHIFT_64 - 1);
    val->lo128.lo = val->lo128.lo << 1;
    val->lo128.hi = (val->lo128.hi << 1) + carryOver;
    val->hi128.lo = (val->hi128.lo << 1) + carryOver2;
    val->hi128.hi = (val->hi128.hi << 1) + carryOver3;
}

void half256(uint256 *val){
    uint64_t carryOver = (val->lo128.hi & 0x1) << (BITSHIFT_64 - 1);
    uint64_t carryOver2 = (val->hi128.lo & 0x1) << (BITSHIFT_64 - 1);
    uint64_t carryOver3 = (val->hi128.hi & 0x1) << (BITSHIFT_64 - 1);
    val->lo128.lo = (val->lo128.lo >> 1) + carryOver;
    val->lo128.hi = (val->lo128.hi >> 1) + carryOver2;
    val->hi128.lo = (val->hi128.lo >> 1) + carryOver3;
    val->hi128.hi = (val->hi128.hi >> 1);
}

int add(uint256 *sum, uint256 a, uint256 b){
    sum->lo128.lo = a.lo128.lo + b.lo128.lo;
    
    uint64_t overflow = a.lo128.lo > 0 && b.lo128.lo > UINT64_MAX - a.lo128.lo ? 1 : 0;
    sum->lo128.hi = a.lo128.hi + b.lo128.hi + overflow;
    
    uint64_t overflow2 = a.lo128.hi > 0 && b.lo128.hi > UINT64_MAX - a.lo128.hi ? 1 : 0;
    sum->hi128.lo = a.hi128.lo + b.hi128.lo + overflow2;

    overflow = a.hi128.lo > 0 && b.hi128.lo > UINT64_MAX - a.hi128.lo ? 1 : 0;
    sum->hi128.hi = a.hi128.hi + b.hi128.hi + overflow;

    return 0;
}

int subtract(uint256 *diff, uint256 a, uint256 b){
    if (compare256(a, b) == -1){
        diff->hi128.hi = 0;
        diff->hi128.lo = 0;
        diff->lo128.hi = 0;
        diff->lo128.lo = 0;
        printf("WARNING: bad subtract\n");
        return -1;
    }

    if (a.lo128.lo < b.lo128.lo){
        uint64_t borrow = UINT64_MAX;
        a.lo128.hi -= 1;
        borrow -= b.lo128.lo;
        a.lo128.lo += (borrow + 1);
        diff->lo128.lo = a.lo128.lo;
    }
    else {
        diff->lo128.lo = a.lo128.lo - b.lo128.lo;
    }

    if (a.lo128.hi < b.lo128.hi){
        uint64_t borrow = UINT64_MAX;
        a.hi128.lo -= 1;
        borrow -= b.lo128.hi;
        a.lo128.hi += (borrow + 1);
        diff->lo128.hi = a.lo128.hi;
    }
    else {
        diff->lo128.hi = a.lo128.hi - b.lo128.hi;
    }

    if (a.hi128.lo < b.hi128.lo){
        uint64_t borrow = UINT64_MAX;
        a.hi128.hi -= 1;
        borrow -= b.hi128.lo;
        a.hi128.lo += (borrow + 1);
        diff->hi128.lo = a.hi128.lo;
    }
    else {
        diff->hi128.lo = a.hi128.lo - b.hi128.lo;
    }

    diff->hi128.hi = a.hi128.hi - b.hi128.hi;
    return 0;
}

/**
 * Multiply 2 128-bit integers using Russian Peasant Algorithm
 * @param product 256 bit product
 * @param a first multiplier
 * @param b second multiplier
 */
int multiply(uint256 *product, uint128 a, uint128 b){
    product->hi128.hi = 0;
    product->hi128.lo = 0;
    product->lo128.hi = 0;
    product->lo128.lo = 0;

    uint256 sum, iter;
    if (compare128(a, b) >= 0){
        convert128To256(&sum, a);
        convert128To256(&iter, b);
    }
    else {
        convert128To256(&sum, b);
        convert128To256(&iter, a);
    }

    while (!(iter.lo128.hi == 0 && iter.lo128.lo == 0)){
        if (iter.lo128.lo & 0x1){
            add(product, sum, *product);
        }

        // double the larger number
        double256(&sum);

        // divide the smaller number
        half256(&iter);
    }

    return 0;
}

/**
 * Find the remainder of a 256 bit integer mod 128 bit integer using 
 *      Russian Peasant Algorithm (for division)
 * @param product 256 bit product
 * @param a first multiplier
 * @param b second multiplier
 */
int modulo(uint128 *rem, uint256 a, uint128 b){
    uint256 b256;
    convert128To256(&b256, b);
    if (compare256(a, b256) == -1){
        rem->hi = a.lo128.hi;
        rem->lo = a.lo128.lo;
        return 0;
    }
    else if (compare256(a, b256) == 0){
        rem->hi = 0;
        rem->lo = 0;
        return 0;
    }

    uint256 divisor = {{0, 0}, {b.hi, b.lo}};

    uint256 temp = {
        {a.hi128.hi / 2, a.hi128.lo / 2}, 
        {a.lo128.hi / 2, a.lo128.lo / 2}
    };

    while (compare256(divisor, temp) < 1){
        double256(&divisor);
    }

    uint256 terminate = {{0, 0}, {0, 0}};

uint16_t bitNum = 0;
    while (compare256(a, b256) > -1){
        while (compare256(a, divisor) > -1){
           if (subtract(&a, a, divisor) == -1){
                return -1;
            }
       }

        half256(&divisor);
    }

    rem->hi = a.lo128.hi;
    rem->lo = a.lo128.lo;

    return 0;
}

/**
 * modulus = prime1 * prime2
 * e = pubExp
 * d = privateExp
 */
void rsa_encrypt(
    uint128 *ciphertext, 
    const char *plaintext, 
    uint128 modulus, 
    uint64_t pubExp){

    uint128 base;

    char *msg = (char *) &base;
    if (strlen(plaintext) < 16){
        memset(msg, 0, 16);
    }

    strcpy(msg, plaintext);

    ciphertext->hi = 0;
    ciphertext->lo = 1;

    uint256 buf, buf2;

    while (pubExp > 0){
        if (pubExp & 0x1){
            multiply(&buf, *ciphertext, base);
            modulo(ciphertext, buf, modulus);
        }

        pubExp = pubExp >> 1;
        multiply(&buf2, base, base);
        modulo(&base, buf2, modulus);
    }
}

void rsa_decrypt(
    uint128 *decrypted, 
    uint128 *ciphertext, 
    uint128 modulus, 
    uint128 privateExp){
        
    decrypted->hi = 0;
    decrypted->lo = 1;

    uint256 privateExp256;
    convert128To256(&privateExp256, privateExp);

    uint256 decrement = {{0, 0}, {0, 0}};

    uint256 buf, buf2;
    uint128 cipherCopy = {ciphertext->hi, ciphertext->lo};

    while (compare256(privateExp256, decrement) > 0){
        if (privateExp256.lo128.lo & 0x1){
            multiply(&buf, *decrypted, cipherCopy);
            modulo(decrypted, buf, modulus);
        }
        half256(&privateExp256);
        multiply(&buf2, cipherCopy, cipherCopy);
        modulo(&cipherCopy, buf2, modulus);
    }
}
