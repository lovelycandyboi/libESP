#include <iostream>
#include <fstream>
#include <cstring>
#include "ctr.h"
using namespace std;
/*
* void CTR::input_nonce() {
        char fileName[100];
        cout << "Enter a file name for Nonce: "; cin >> fileName;
        BlockFile_to_4X4matrix(fileName, aes.get_aesKeyBlock());
        memcpy(nonce, aes.get_aesKeyBlock(), sizeof(byte) * 16);
}

void CTR::encrypt() {
        byte* plainText = aes.get_plainTextBlock();
        byte cipherText[16];
        byte keystream[16];

        int numBlocks = aes.getNumBlocks();
        for (int i = 0; i < numBlocks; i++) {
            memcpy(counter + counter_size, &i, sizeof(int));
            aes.AES_encryption();
            memcpy(keystream, aes.get_cipherTextBlock(), sizeof(byte) * 16);

            for (int j = 0; j < 16; j++)    keystream[j] ^= nonce[j];
            for (int j = 0; j < 16; j++)    cipherText[j] = plainText[i * 16 + j] ^ keystream[j];

            fwrite(cipherText, sizeof(byte), 16, stdout);
        }
}

void CTR::decrypt() {
        byte* cipherText = aes.get_cipherTextBlock();
        byte plainText[16];
        byte keystream[16];

        int numBlocks = aes.getNumBlocks();
        for (int i = 0; i < numBlocks; i++) {
            memcpy(counter + counter_size, &i, sizeof(int));
            aes.AES_encryption();
            memcpy(keystream, aes.get_cipherTextBlock(), sizeof(byte) * 16);

            for (int j = 0; j < 16; j++) keystream[j] ^= nonce[j];
            for (int j = 0; j < 16; j++) plainText[j] = cipherText[i * 16 + j] ^ keystream[j];
            
            fwrite(plainText, sizeof(byte), 16, stdout);
        }
    }

*/
