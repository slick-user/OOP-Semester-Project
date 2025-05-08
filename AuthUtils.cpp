#include "AuthUtils.h"

void hashPassword(const char* input, char* output) {
    // Simple XOR-based hashing (custom method)
    int key = 7;  // Just a custom key
    int len = strlen(input);
    for (int i = 0; i < len; ++i) {
        output[i] = input[i] ^ key;
    }
    output[len] = '\0';
}

