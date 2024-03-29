#include "../include/util.h"

uint8_t Galua_mul(uint8_t a, uint8_t b) {
    uint8_t result = 0;

    for (int i = 0; i < 8; i++) {
        if (b & 1) result ^= a;
        uint8_t msb = a & 0x80;
        a <<= 1;
        if (msb) a ^= 0x1b; // Reduction polynomial for GF(2^8)
        b >>= 1;
    }

    return result;
}

int Euler_phi_function(int n) {
	int result = n;

	for (int i = 2; i * i <= n; i++) {
		if (n % i == 0)		result = result - result / i;
		while (n % i == 0)	n /= i;
	}
	if (n != 1) result = result - result / n;

	return result;
}

bool BlockFile_to_4X4matrix(const char* plainTextFile, uint8_t(*plainText)[4]) {

    const int row = 4;
    const int col = 4;

    std::ifstream inputfile(plainTextFile);
    printf("file name:%s\n", plainTextFile);
    
    if (!inputfile.is_open()) return false;

    for (int r = 0; r < row; r++)
        for (int c = 0; c < col; c++)
            inputfile >> plainText[r][c];

    inputfile.close();

    return true;
}

void fft(vector<complex<double>>& a, bool inv) {
    int n = a.size(), j = 0;
    
    vector<complex<double>> roots(n / 2);

    for (int i = 1; i < n; i++) {
        int bit = (n >> 1);

        while (j >= bit) {
            j -= bit;
            bit >>= 1;
        }
        j += bit;

        if (i < j) { swap(a[i], a[j]); }
    }

    double ang = 2 * acos(-1) / n * (inv ? -1 : 1);
    for (int i = 0; i < n / 2; i++) {
        roots[i] = complex<double>(cos(ang * i), sin(ang * i));
    }

    for (int i = 2; i <= n; i <<= 1) {

        int step = n / i;

        for (int j = 0; j < n; j += i) {
            for (int k = 0; k < i / 2; k++) {
                complex<double> u = a[j + k], v = a[j + k + i / 2] * roots[step * k];
                a[j + k] = u + v;
                a[j + k + i / 2] = u - v;
            }
        }

    }

    if (inv) { for (int i = 0; i < n; i++) a[i] /= n; }
}
vector<long long> fft_multiply(vector<long long>& v, vector<long long>& w) {
    vector<complex<double>> fv(v.begin(), v.end()), fw(w.begin(), w.end());

    int n = 2;
    while (n < v.size() + w.size()) n <<= 1;

    fv.resize(n); fw.resize(n);
    fft(fv, false);
    fft(fw, false);

    for (int i = 0; i < n; i++)
        fv[i] *= fw[i];

    fft(fv, true);

    vector<long long> ret(n);

    for (int i = 0; i < n; i++)
        ret[i] = (long long)round(fv[i].real());

    return ret;
}

void hexStr2int(char* src, int* dst) {
    *dst = 0;
    for (int i = 0; src[i] != NULL; i++) {
        if (src[i] >= '0' && src[i] <= '9')			*dst += (src[i] - '0');
        else if (src[i] >= 'a' && src[i] <= 'f')	*dst += (src[i] - 'a' + 10);
        else if (src[i] >= 'A' && src[i] <= 'F')	*dst += (src[i] - 'A' + 10);
        *dst <<= 4;
    }
    *dst >>= 4;
}
void decStr2int(char* src, int* dst) {
    *dst = 0;
    for (int i = 0; src[i] != NULL; i++) {
        *dst += (src[i] - '0');
        *dst *= 10;
    }
    *dst /= 10;
}

