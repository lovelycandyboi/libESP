#include <stdio.h>
#include <string.h>
#include "aes.h"

void aes128::sub_byte(byte(*cipherText)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] = sbox[(cipherText[i][j] & 0xF0) >> 4][cipherText[i][j] & 0x0F];
}

void aes128::inverse_sub_byte(byte(*cipherText)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] = inverseSbox[(cipherText[i][j] & 0xF0) >> 4][cipherText[i][j] & 0x0F];
}

void aes128::shift_row(byte(*cipherText)[4]) {
    word tempRow[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(cipherText[i], tempRow[i]);
    for (int i = 0; i < 4; i++) circuitShift_left(tempRow[i], i);
    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempRow[i], cipherText[i]);
}

void aes128::inverse_shift_row(byte(*cipherText)[4]) {
    word tempRow[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(cipherText[i], tempRow[i]);
    for (int i = 0; i < 4; i++) circuitShift_right(tempRow[i], i);
    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempRow[i], cipherText[i]);
}

void aes128::mix_columns(byte(*cipherText)[4]) {
    byte tempResult[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tempResult[i][j] = 0;
            for (int k = 0; k < 4; k++) tempResult[i][j] ^= Galua_mul(constantMatrix[i][k], cipherText[k][j]) % 0b100011011;
        }
    }


    memcpy(cipherText, tempResult, sizeof(tempResult));
}

void aes128::inverse_mix_columns(byte(*cipherText)[4]) {
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

void aes128::add_round_key(byte(*cipherText)[4], byte(*key)[4]) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cipherText[i][j] ^= key[i][j];
}

void aes128::key_scheduling(byte(*key)[4], byte(*RoundKey)[4][4]) {
    word tempWord[4];
    for (int i = 0; i < 4; i++) union_4bytes_To_Word(key[i], tempWord[i]);
    for (int i = 0; i < 4; i++) div_Word_To_4bytes(tempWord[i], RoundKey[0][i]);               //first round key
    for (int roundNumb = 1; roundNumb <= 10; roundNumb++) {
        tempWord[0] ^= G(tempWord[3], roundNumb);
        tempWord[1] ^= tempWord[0];
        tempWord[2] ^= tempWord[1];
        tempWord[3] ^= tempWord[2];
        for (int i = 0; i < 4; i++)
            div_Word_To_4bytes(tempWord[i], RoundKey[roundNumb][i]);   //N'th roundKey
    }
}

word aes128::G(word inputWord, int roundNumb) {
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

byte** aes128::encrypt(byte(*plainText)[4], byte(*roundKey)[4][4]) {
    byte cipherText[4][4] = { 0, };
    memcpy(cipherText, plainText, sizeof(byte) * 16);
    
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
    return (byte**)cipherText;
}

void aes128::AES_encryption() {
    memcpy(this->cipherTextBlock, this->encrypt(this->plainTextBlock, this->roundKey), sizeof(byte) * 16);
}

byte** aes128::decrypt(byte(*cipherText)[4], byte(*roundKey)[4][4]) {
    byte plainText[4][4] = { 0, };
    memcpy(plainText, cipherText, sizeof(byte) * 16);

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

    return (byte**)plainText;
}

void aes128::AES_decryption() {
    memcpy(this->plainTextBlock, this->decrypt(this->cipherTextBlock, this->roundKey), sizeof(byte) * 16);
}

void aes128::print_massage() {
    printf("plainTextBlock->");
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            printf("%c", plainTextBlock[r][c]);
    printf("\n");

    printf("cipherTextBlock->");
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            printf("%c", cipherTextBlock[r][c]);
    printf("\n");
}
void aes128::print_key() {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            printf("%c", aesKeyBlock[r][c]);
    printf("\n");
}

void aes128::input_massage() {
    char textFileName[100];
    do {
        printf("write textFile:"); std::cin >> textFileName;
    } while (!BlockFile_to_4X4matrix(textFileName, plainTextBlock));
}
void aes128::input_key() {
    char keyFileName[100];
    do {
        printf("write keyFile:");  std::cin >> keyFileName;
    } while (!BlockFile_to_4X4matrix(keyFileName, aesKeyBlock));
}

byte* aes128::get_plainTextBlock(){
    byte return_plainText[16];
    memcpy(return_plainText, this->plainTextBlock, sizeof(byte) * 16);
    return (byte*)return_plainText;
}

byte* aes128::get_cipherTextBlock() {
    byte return_cipherText[16];
    memcpy(return_cipherText, this->cipherTextBlock, sizeof(byte) * 16);
    return (byte*)return_cipherText;
}