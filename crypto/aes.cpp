#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"

void aes::sub_byte(byte(*cipherText)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] = sbox[(cipherText[i][j] & 0xF0) >> 4][cipherText[i][j] & 0x0F];
}

void aes::inverse_sub_byte(byte(*cipherText)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] = inverseSbox[(cipherText[i][j] & 0xF0) >> 4][cipherText[i][j] & 0x0F];
}

void aes::shift_row(byte(*cipherText)[4]) {
    word tempRow[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(cipherText[i], tempRow[i]);
    for (int i = 0; i < 4; i++) circuitShift_left(tempRow[i], i);
    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempRow[i], cipherText[i]);
}

void aes::inverse_shift_row(byte(*cipherText)[4]) {
    word tempRow[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(cipherText[i], tempRow[i]);
    for (int i = 0; i < 4; i++) circuitShift_right(tempRow[i], i);
    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempRow[i], cipherText[i]);
}

void aes::mix_columns(byte(*cipherText)[4]) {
    byte tempResult[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tempResult[i][j] = 0;
            for (int k = 0; k < 4; k++) tempResult[i][j] ^= Galua_mul(constantMatrix[i][k], cipherText[k][j]) % 0b100011011;
        }
    }


    memcpy(cipherText, tempResult, sizeof(tempResult));
}

void aes::inverse_mix_columns(byte(*cipherText)[4]) {
    byte tempResult[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tempResult[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                tempResult[i][j] ^= Galua_mul(inverseConstantMatrix[i][k], cipherText[k][j]) % 0b100011011;
            }
        }
    }

    memcpy(cipherText, tempResult, sizeof(tempResult));
}

void aes::add_round_key(byte(*cipherText)[4], byte(*key)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] ^= key[i][j];
}

void aes::key_scheduling(byte(*key)[4], byte(*RoundKey)[4][4]) {

    word tempWord[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(key[i], tempWord[i]);

    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempWord[i], RoundKey[0][i]);               //first round key

    for (int roundNumb = 1; roundNumb <= 10; roundNumb++) {
        tempWord[0] ^= G(tempWord[3], roundNumb);
        tempWord[1] ^= tempWord[0];
        tempWord[2] ^= tempWord[1];
        tempWord[3] ^= tempWord[2];

        for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempWord[i], RoundKey[roundNumb][i]);   //N'th roundKey
    }

}

word aes::G(word inputWord, int roundNumb) {
    //=============================================================================================RotWord
    word outputWord = inputWord;
    circuitShift_left(outputWord, 1);

    //=============================================================================================SubWord
    byte temp[4];
    div_Word_To_4bytes(outputWord, temp);
    for (int i = 0; i < 4; i++) { temp[i] = sbox[(temp[i] & 0xF0) >> 4][temp[i] & 0x0F]; }

    //=============================================================================================addRcon     
    union_4bytes_To_Word(temp, outputWord);
    outputWord ^= Rcond[roundNumb];

    return outputWord;
}

void aes::encrypt(byte(*cipherText)[4], byte(*roundKey)[4][4]) {
    add_round_key(cipherText, roundKey[0]);
    for (int roundNumb = 1; roundNumb < 10; roundNumb++) {
        sub_byte(cipherText);
        shift_row(cipherText);
        mix_columns(cipherText);
        add_round_key(cipherText, roundKey[roundNumb]);
    }
    sub_byte(cipherText);
    shift_row(cipherText);
    add_round_key(cipherText, roundKey[10]);
}

void aes::AES_encryption(byte(*plainText)[4], byte(*key)[4]) {
    byte roundKey[11][4][4] = { 0, };
    key_scheduling(key, roundKey);
    encrypt(plainText, roundKey);
}

void aes::decrypt(byte(*plainText)[4], byte(*roundKey)[4][4]) {
    add_round_key(plainText, roundKey[10]);
    for (int roundNumb = 1; roundNumb < 10; roundNumb++) {
        inverse_shift_row(plainText);
        inverse_sub_byte(plainText);
        add_round_key(plainText, roundKey[10 - roundNumb]);
        inverse_mix_columns(plainText);
    }

    inverse_shift_row(plainText);
    inverse_sub_byte(plainText);
    add_round_key(plainText, roundKey[0]);
}

void aes::AES_decryption(byte(*cipherText)[4], byte(*key)[4]) {
    byte roundKey[11][4][4] = { 0, };
    key_scheduling(key, roundKey);

    decrypt(cipherText, roundKey);
}

void aes::print_massage() {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            printf("%c", plainTextBlock[r][c]);
    printf("\n");
}
void aes::print_key() {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            printf("%c", aesKeyBlock[r][c]);
    printf("\n");
}

void aes::input_massage() {
    char textFileName[20];
    do {
        printf("write textFile:"); std::cin >> textFileName;
    } while (!BlockFile_to_4X4matrix(textFileName, plainTextBlock));
}

void aes::input_key() {
    char keyFileName[20];
    do {
        printf("write keyFile:");  std::cin >> keyFileName;
    } while (!BlockFile_to_4X4matrix(keyFileName, aesKeyBlock));
}

void aes::test_aes() {
    this->input_massage();
    this->input_key();
    AES_encryption(plainTextBlock, aesKeyBlock);
    printf("aes128 encrypt result->");
    this->print_massage();

    AES_decryption(plainTextBlock, aesKeyBlock);
    printf("aes128 decrypt result->");
    this->print_massage();
}