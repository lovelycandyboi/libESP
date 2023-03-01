#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <complex>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm>

using namespace std;

//const double PI = acos(-1);
#define byte uint8_t
#define word uint32_t

#define circuitShift_left(byteVar, shiftByteNumb)                                                                                                   \
        byteVar = (byteVar << (8 * (shiftByteNumb % sizeof(byteVar)))) | (byteVar >> (8 * (sizeof(byteVar) - (shiftByteNumb % sizeof(byteVar)))));                                       

#define circuitShift_right(byteVar, shiftByteNumb)                                                                                                  \
        byteVar = (byteVar >> (8 * (shiftByteNumb % sizeof(byteVar)))) | (byteVar << (8 * (sizeof(byteVar) - (shiftByteNumb % sizeof(byteVar)))));                                       

#define union_4bytes_To_Word(byteList, Word)                                                                                                        \
        do{                                                                                                                                         \
            try{                                                                                                                                    \
                if (sizeof(Word) == 4 && sizeof(byteList) == 4) Word = byteList[0] << 24 | byteList[1] << 16 | byteList[2] << 8 | byteList[3];      \
                else throw(string("union_tbytes_To_Word"));                                                                                         \
            }                                                                                                                                       \
            catch (string errorMacro) {printf("error in util.h : %s has wrong parameter\n", errorMacro);}                                           \
        }while(0)


#define div_Word_To_4bytes(Word, byteList)                                                                                                          \
        do{                                                                                                                                         \
            try{                                                                                                                                    \
                if (sizeof(Word) == 4 && sizeof(byteList) == 4) {                                                                                   \
                    byteList[0] = (byte)(Word >> (8 * 3));                                                                                          \
                    byteList[1] = (byte)(Word >> (8 * 2));                                                                                          \
                    byteList[2] = (byte)(Word >> (8 * 1));                                                                                          \
                    byteList[3] = (byte)(Word >> (8 * 0));                                                                                          \
                }                                                                                                                                   \
                else throw(string("div_Word_To_4bytes"));                                                                                           \
            }                                                                                                                                       \
            catch (string errorMacro) {printf("error in util.h : %s has wrong parameter\n", errorMacro);}                                           \
        }while(0)

byte Galua_mul(byte a, byte b);

int Euler_phi_function(int n);

bool BlockFile_to_4X4matrix(const char* plainTextFile, uint8_t(*plainText)[4]);

void fft(vector<complex<double>>&a, bool inv);
vector<long long> fft_multiply(vector<long long>&v, vector<long long>&w);

void hexStr2int(char* src, int* dst);
void decStr2int(char* src, int* dst);

#endif