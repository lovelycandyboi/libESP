#include <iostream>
#include <fstream>
#include <cstring>
#include "aes.h"

using namespace std;

class CTR {
private:
    aes128 aes;
    byte nonce[16];
    byte counter[16];
    int counter_size;
public:
    CTR() {
        counter_size = 8;
    }

    void input_nonce();
    void encrypt();
    void decrypt();
};