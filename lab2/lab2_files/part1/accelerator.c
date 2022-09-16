#include "rsa.h"

#define BITSHIFT_32 32
#define HIGH_MASK_32 0xFFFF0000
#define LOW_MASK_32 0x0000FFFF

#define CARRY_OVER_BIT 0x8000000000000000
#define BITSHIFT_64 64

uint256 * createUint256(){
    return (uint256 *)malloc(sizeof(uint256));
}

int convert128To256(uint256 *result, uint128 val){
    // result = (uint256 *) createUint256();

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
    
    if (hiCompare == 1 || (hiCompare == 0 && lowCompare == 1))
        return 1;

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
    uint8_t carryOver = (val->lo128.hi & 0x1) << (BITSHIFT_64 - 1);
    uint8_t carryOver2 = (val->hi128.lo & 0x1) << (BITSHIFT_64 - 1);
    uint8_t carryOver3 = (val->hi128.hi & 0x1) << (BITSHIFT_64 - 1);
    val->lo128.lo = val->lo128.lo >> 1;
    val->lo128.hi = (val->lo128.hi >> 1) + carryOver;
    val->hi128.lo = (val->hi128.lo >> 1) + carryOver2;
    val->hi128.hi = (val->hi128.hi >> 1) + carryOver3;
}

void add(uint256 *sum, uint256 a, uint256 b){
    sum->lo128.lo = a.lo128.lo + b.lo128.lo;
    
    uint64_t overflow = sum->lo128.lo < a.lo128.lo && sum->lo128.lo < b.lo128.lo ? 1 : 0;
    sum->lo128.hi = a.lo128.hi + b.lo128.hi + overflow;
    
    uint64_t overflow2 = sum->lo128.hi < a.lo128.hi && sum->lo128.lo < b.lo128.hi ? 1 : 0;
    sum->hi128.lo = a.hi128.lo + b.hi128.lo + overflow2;

    overflow = sum->hi128.lo < a.hi128.lo && sum->hi128.lo < b.hi128.lo ? 1 : 0;
    sum->hi128.hi = a.hi128.hi + b.hi128.hi + overflow;
}

int subtract(uint256 *diff, uint256 a, uint256 b){
    // printf("a: %lu %lu b: %lu %lu\n", a.lo128.hi, a.lo128.lo, b.lo128.hi, b.lo128.lo);
    if (compare256(a, b) == -1){
        diff->hi128.hi = 0;
        diff->hi128.lo = 0;
        diff->lo128.hi = 0;
        diff->lo128.lo = 0;
        printf("WARNING: bad subtract\n");
        return -1;
    }

    if (a.lo128.lo < b.lo128.lo){
        uint64_t borrow = 0xffffffffffffffffLL;
        a.lo128.hi -= 1;
        borrow -= b.lo128.lo;
        a.lo128.lo += (borrow + 1);
        diff->lo128.lo = a.lo128.lo;
    }
    else {
        diff->lo128.lo = a.lo128.lo - b.lo128.lo;
    }

    if (a.lo128.hi < b.lo128.hi){
        uint64_t borrow = 0xffffffffffffffffLL;
        a.hi128.lo -= 1;
        borrow -= b.lo128.hi;
        a.lo128.hi += (borrow + 1);
        diff->lo128.hi = a.lo128.hi;
    }
    else {
        diff->lo128.hi = a.lo128.hi - b.lo128.hi;
    }

    if (a.hi128.lo < b.hi128.lo){
        uint64_t borrow = 0xffffffffffffffffLL;
        a.hi128.hi -= 1;
        borrow -= b.hi128.lo;
        a.hi128.lo += (borrow + 1);
        diff->lo128.hi = a.hi128.lo;
    }
    else {
        diff->hi128.lo = a.hi128.lo - b.hi128.lo;
    }

    diff->hi128.hi = a.hi128.hi - b.hi128.hi;
    // printf("subtract result: %lx %lx\n", diff->hi, diff->lo);
    return 0;
}

// Fix to store 2 128 bit integers
int multiply(uint256 *product, uint128 a, uint128 b){
    product->hi128.hi = 0;
    product->hi128.lo = 0;
    product->lo128.hi = 0;
    product->lo128.lo = 0;

    uint256 sum;
    uint128 *smaller;
    if (compare128(a, b) >= 0){
        convert128To256(&sum, a);
        smaller = &b;
    }
    else {
        convert128To256(&sum, b);
        smaller = &a;
    }

    uint64_t remainder = smaller->hi;
    uint64_t val = smaller->lo;

    // printf("sum: %lx %lx\n", sum.hi, sum.lo);
    // printf("remainder: %lx, val: %lx\n", remainder, val);

    while (!(val == 0 && remainder == 0)){
        if ((val % 2) == 1){
            add(product, sum, *product);
            // uint128 overflow = {0, add(product[1], sum, *product[1])};
            // add(product[0], overflow, *product[0]);
            // printf("add result: %lu %lu\n", product->hi, product->lo);
        }

        // double the larger number
        double256(&sum);
// printf("new sum: %lu %lu\n", sum.hi, sum.lo);

        // divide the smaller number
        val = val >> 1;
        val += (remainder % 2) << (BITSHIFT_64 - 1);
        remainder = remainder >> 1;
// printf("new remainder: %lx  new val: %lx\n", remainder, val);
    }

    return 0;
}

int modulo(uint128 *rem, uint256 a, uint128 b){
    uint256 *divisor = createUint256();
    divisor->lo128 = b;

    // printf("divisor: %lx %lx\n", divisor.hi, divisor.lo);
    uint256 temp = {
        {a.hi128.hi / 2, a.hi128.lo / 2}, 
        {a.lo128.hi / 2, a.lo128.lo / 2}
    };
    // uint128 temp[2] = {{a[0]->hi / 2, a[0]->lo / 2}, {a[1]->hi / 2, a[1]->lo / 2}};

    uint64_t carryOver, carryOver2, carryOver3;

    while (compare256(*divisor, temp) == -1){
        double256(divisor);
    }

    convert128To256(&temp, b);
    // printf("divisor: %lx %lx\n", divisor.hi, divisor.lo);

    uint256 terminate = {{0, 0}, {0, 0}};

    while (compare256(a, temp) > -1){
        if (compare256(a, *divisor) > -1){
// printf("a: %lu %lu divisor: %lu %lu\n", a.lo128.hi, a.lo128.lo, divisor->lo128.hi, divisor->lo128.lo);
            if (subtract(&a, a, *divisor) == -1){
                return -1;
            }
            // printf("subtracted divisor: %lu %lu\n", divisor.hi, divisor.lo);
        }
        half256(divisor);

        if (compare256(*divisor, terminate) == 0){
            break;
        }
    }

    rem->hi = a.lo128.hi;
    rem->lo = a.lo128.lo;

    free(divisor);

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
    uint64_t pubExp,
    uint64_t msgLen){
    printf("encrypting\n");

    // for (int i = 0; i < msgLen; i++){
    //     printf("%x ", plaintext[i]);
    // }
    // printf("\n");

// uint128 *temp = (uint128 *)plaintext;
// printf("plaintext: %016lx %016lx\n", temp->hi, temp->lo);
    // fix this later
    if (msgLen < 8){
        char *msg = (char *) ciphertext;
        memset(msg, 0, 16);
        // printf("plaintext: %016lx %016lx\n", plaintext->hi, plaintext->lo);
        // memset(&msg[8 - msgLen], *plaintext, msgLen);
        for (int i = 0; i < msgLen; i++){
            memset(&msg[8 + i], plaintext[i], 1);
        }
    }

    uint128 original = {ciphertext->hi, ciphertext->lo};

    printf("plaintext: %016lu %016lu\n", ciphertext->hi, ciphertext->lo);
    printf("mod: %lu %lu\n", modulus.hi, modulus.lo);

    uint256 buf, buf2, pubExp256, terminate;
    pubExp--;
/*
    pubExp256.lo128.lo = pubExp;

    while (compare256(pubExp256, terminate) > 0){
        // printf("im here\n");
        half256(&pubExp256);
        multiply(&buf2, original, original);
        modulo(&original, buf2, modulus);
        // printf("buf2: %lu %lu\n", buf2.lo128.hi, buf2.lo128.lo);

        if (pubExp256.lo128.lo & 0x1){
            multiply(&buf, *ciphertext, original);
            modulo(ciphertext, buf, modulus);
            // printf("buf: %lu %lu\n", buf.lo128.hi, buf.lo128.lo);
        }

    }
    */

///*
    while (pubExp > 0){
        multiply(&buf, *ciphertext, original);
        pubExp--;
        modulo(ciphertext, buf, modulus);
// printf("encr mod: %lx %lx\n", ciphertext->hi, ciphertext->lo);
    } // for small case, result is x20
//*/

    printf("ciphertext: %lx %lx\n", ciphertext->hi, ciphertext->lo);
    // ciphertext = {0x900be935e0beb101LL, 0x2700000000000000LL};

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

    uint256 privateExp256;
    convert128To256(&privateExp256, privateExp);

    uint256 decrement = {{0, 0}, {0, 1}};
    printf("ciphertext: %lx %lx\n", ciphertext->hi, ciphertext->lo);
    printf("mod: %lx %lx\n", modulus.hi, modulus.lo);

    // subtract(&privateExp256, privateExp256, decrement);

    uint256 buf, buf2;
    uint128 cipherCopy = {ciphertext->hi, ciphertext->lo};

    while (compare256(privateExp256, decrement) != -1){
        // printf("im here\n");
        half256(&privateExp256);
        multiply(&buf2, cipherCopy, cipherCopy);
        modulo(&cipherCopy, buf2, modulus);
        // printf("buf2: %lu %lu\n", buf2.lo128.hi, buf2.lo128.lo);

        if (privateExp256.lo128.lo & 0x1){
            multiply(&buf, *decrypted, cipherCopy);
            modulo(decrypted, buf, modulus);
            // printf("buf: %lu %lu\n", buf.lo128.hi, buf.lo128.lo);
        }

    }

/*
    while (compare256(privateExp256, decrement) == 1){
        multiply(buf, *decrypted, *ciphertext);
        // printf("decr mult: %lu %lu\n", buf->lo128.hi, buf->lo128.lo);
        subtract(&privateExp256, privateExp256, decrement);
        if (modulo(decrypted, *buf, modulus) == -1){
            printf("FUCK\n");
            break;
        }

        // if (privateExp256.lo128.lo % 100000 == 0){
        //     printf("privateExp countdown: %lu %lu %lu %lu\n", 
        //         privateExp256.hi128.hi,
        //         privateExp256.hi128.lo,
        //         privateExp256.lo128.hi,
        //         privateExp256.lo128.lo);
        // }
        // printf("decr mod: %lu %lu\n", decrypted->hi, decrypted->lo);
    }
*/
    printf("privateExp: %lx %lx\n", privateExp256.lo128.hi, privateExp256.lo128.lo);
    printf("decrypted: %016lx %016lx\n", decrypted->hi, decrypted->lo);
    
    // char *decrypted_text = (char*)decrypted;
    // for (int i = 0; i < 16; i++){
    //     printf("%u: %x\n", i, decrypted_text[i] & 0xFF);
    // }

    if (msgLen < 8){
        uint64_t temp = decrypted->hi;
        decrypted->hi = decrypted->lo;
        decrypted->lo = temp;
    }

    // free(buf);
}
