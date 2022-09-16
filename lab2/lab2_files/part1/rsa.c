#include <assert.h>

#include "rsa.h"

int main() {
/* Private-Key: (128 bit)                                                                                                                                         */
/* modulus: */
/*    00:e0:37:d3:5a:8b:16:0e:b7:f1:19:19:bf:ef:44: */
/*    09:17 */
/* publicExponent: 65537 (0x10001) */
/* privateExponent: */
/*    00:ca:b1:0c:ca:a4:43:7b:67:11:c9:77:a2:77:fe: */
/*    00:a1 */
/* prime1: 18125493163625818823 (0xfb8aafffd4b02ac7) */
/* prime2: 16442969659062640433 (0xe43129c94cf45f31) */
/* exponent1: 5189261458857000451 (0x4803f5cd8dcbfe03) */
/* exponent2: 12850891953204883393 (0xb2578a24fdb3efc1) */
/* coefficient: 10155582946292377246 (0x8cefe0e210c5a69e) */

    //DO NOT MODIFY
    uint128 modulus = {0xe037d35a8b160eb7LL,  0xf11919bfef440917LL};
    uint128 privateExp = {0x00cab10ccaa4437b67LL,  0x11c977a277fe00a1LL};
    uint64_t pubExp = 65537;
    const char plaintext[] = "H";
    uint128 ciphertext;
    uint128 decrypted;
    //END DO NOT MODIFY

    // Small example
    // uint128 modulus = {0,  31313};
    // uint128 privateExp = {0,  4423};
    // uint64_t pubExp = 7;
    // const char plaintext[] = "Hi";
    // uint128 ciphertext;
    // uint128 decrypted;

    // Even smaller example
    // uint128 modulus = {0,  247};
    // uint128 privateExp = {0, 31};
    // uint64_t pubExp = 7;
    // const char plaintext[] = "H";
    // uint128 ciphertext;
    // uint128 decrypted;

    /* YOUR CODE HERE: Invoke your RSA acclerator, write the encrypted output of plaintext to ciphertext */ 
    uint64_t msgLen = strlen(plaintext);
    // printf("%x\n", plaintext);
    rsa_encrypt(&ciphertext, &plaintext, modulus, pubExp, msgLen);

    //DO NOT MODIFY
    char *encrypted_text = (char*)&ciphertext;
    printf("encrypted=%s\n", encrypted_text);
    //END DO NOT MODIFY

    /* YOUR CODE HERE: Invoke your RSA acclerator, write the decrypted output of ciphertext to decrypted */
    // printf("decr mod: %lu %lu\n", privateExp.hi, privateExp.lo);
    rsa_decrypt(&decrypted, &ciphertext, modulus, privateExp, msgLen);

    //DO NOT MODIFY
    char *decrypted_text = (char*)&decrypted;
    printf("decrypted=%s\n", decrypted_text);
    assert(strcmp(plaintext, decrypted_text) == 0);
}
