#include <stdio.h>
#include <stdlib.h>
#include "../include/cli.h"
#include "../crypto/aes.h"
#include "../crypto/aes256gcm.h"

extern CLI_BLOCK* CLI_BLOCK_HEAD;
extern char VV_stringArgs[10][CLI_ARG_BUFFER_SIZE];
extern int VV_stringArgsNumb;
extern bool quitFlag;


#ifdef WIN32

#include <windows.h>
int print_curpathFiles() {
	WIN32_FIND_DATAW fileData;
	HANDLE hFind;
	wchar_t path[MAX_PATH];

	GetCurrentDirectoryW(MAX_PATH, path);
	wprintf(L"Current directory: %s\n", path);

	wcscat_s(path, MAX_PATH, L"\\*.*");

	hFind = FindFirstFileW(path, &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		printf("can not open cur directory\n");
		return 1;
	}

	do {
		if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) wprintf(L"%s\n", fileData.cFileName);
	} while (FindNextFileW(hFind, &fileData) != 0);

	FindClose(hFind);
	return 0;
}
#else
#include <dirent.h>

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int print_curpathFiles() {
	DIR* dir;
	struct dirent* ent;
	char path[1024];

	getcwd(path, 1024);
	printf("Current directory: %s\n", path);

	dir = opendir(".");
	if (dir == NULL) {
		printf("can not open cur directory\n");
		return 1;
	}

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type != DT_DIR && ent->d_name[0] != '.') {
			printf("%s\n", ent->d_name);
		}
	}

	closedir(dir);
	return 0;
}
#endif

void print_linkedBlockList() {
	for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next)
		printf("%s\n", blockIter->command);
}

DEF_CLI(manual, "help") {
	printf("==============Cmd List==============\n");
	print_linkedBlockList();
	printf("==========Cur dir file List=========\n");
	print_curpathFiles();
}

DEF_CLI(print_test, "print_test VV with VV") {
	printf("%s %s\n", VV_stringArgs[0], VV_stringArgs[1]);
	printf("test success!\n");
}

DEF_CLI(Euler_phi_function, "test euler") {
	int N;
	while (true) {
		printf("input Val:");
		std::cin >> N;
		if (N != -1) printf("numb of disjoint:%d\n", Euler_phi_function(N));
		else break;
	}
	getchar();
}

DEF_CLI(Euler_phi, "test euler VV") {
	int N = 0;
	decStr2int(VV_stringArgs[0], &N);
	printf("disjointNumb of %d :%d\n", N, Euler_phi_function(N));
}

DEF_CLI(AES128_test, "test AES") {
	aes128* test = new aes128();
	test->AES_encryption();

	byte cipherText[16];
	memcpy(cipherText, test->get_cipherTextBlock(), sizeof(byte) * 16);
	
	byte plainText[16];
	memcpy(plainText, test->get_plainTextBlock(), sizeof(byte) * 16);
	
	for (int i = 0; i < 16; i++)	printf("%c", cipherText[i]);	printf("\n");
	for (int i = 0; i < 16; i++)	printf("%c", plainText[i]);		printf("\n");
}


DEF_CLI(fft_test, "test fft") {
	string str1, str2;
	printf("input 1st para:");	std::cin >> str1;
	printf("input 2nd para:");  std::cin >> str2;

	if (str1 == "0" || str2 == "0") { cout << 0 << "\n"; return; }

	vector<long long> a(str1.size()), b(str2.size());

	for (int i = 0; i < str1.size(); i++)	a[str1.size() - i - 1] = str1[i] - '0';

	for (int i = 0; i < str2.size(); i++)	b[str2.size() - i - 1] = str2[i] - '0';

	vector<long long> ret = fft_multiply(a, b);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < str1.size() + str2.size() - 1; i++) {
		ret[i + 1] += ret[i] / 10;
		ret[i] = ret[i] % 10;
	}

	int cnt = ret.size() - 1;

	while (ret[cnt] == 0 && cnt != 0) cnt--;

	printf("mul result->");
	for (int i = cnt; i >= 0; i--) cout << ret[i];
	cout << "\n";

	getchar();
	return;
}

DEF_CLI(gcm_mode, "test_aes256 gcm_mode") {
	const size_t BLOCK_SIZE = 64;
	const size_t NUM_BLOCKS = 2;

	aes256gcm_key_set   x = { {0,},{0,},0 };                                            // key[32], nonce[12], stream_key
	aes256gcm_ghash_set g = { {0,},{0,},{0, }, {0, }, 0 };                              // Ghash[32], enc_tag_result[32], dec_tag_result[32], ad[32][4], ad_size
	g.ad_size = 40;
	const size_t outlen = NUM_BLOCKS * BLOCK_SIZE + 132;                                // output size is the number of blocks times block size
	uint8_t testText[outlen] = { 0, };
	x.stream_key = (uint8_t*)malloc(outlen * sizeof(uint8_t));                          // Allocate memory for the output keyStream
	g.ad = (uint8_t*)malloc(g.ad_size * sizeof(uint8_t));

	for (int i = 0; i < outlen; i++) {
		testText[i] = i;
	}
	printf("plainText messgae:\n");
	for (int i = 0; i < outlen; i++)    printf("%02x ", testText[i]);
	printf("\n\n");

	aes256gcm_ctx state;

	aes256gcm_init(&state, x.key, x.nonce);
	printf("Ghash init before:\n");
	for (int i = 0; i < AES256gcm_HASHBYTES; i++)
		printf("%02x ", g.Ghash[i]);
	printf("\n\n");



	aes256gcm_prf(g.Ghash, AES256gcm_HASHBYTES, x.key, x.nonce); //initial Ghash_tag && key expansion
	printf("Ghash init after:\n");
	for (int i = 0; i < AES256gcm_HASHBYTES; i++)
		printf("%02x ", g.Ghash[i]);
	printf("\n\n");



	aes256gcm_squeezeblocks(x.stream_key, outlen, &state);
	printf("Keystream:\n");
	for (int i = 0; i < outlen; i++)    printf("%02x ", x.stream_key[i]);
	printf("\n\n");



	aes256gcm_encrypt(testText, x.stream_key, outlen, &g); // Encrypt the output
	printf("Encrypted text:\n");
	for (int i = 0; i < outlen; i++)    printf("%02x ", testText[i]);
	printf("\n\n");
	printf("after enc Ghash:\n");
	for (int i = 0; i < AES256gcm_HASHBYTES; i++)
		printf("%02x ", g.enc_tag_result[i]);
	printf("\n\n");


	if (aes256gcm_decrypt(testText, x.stream_key, outlen, &g)) {
		printf("Decrypted text:\n");
		for (int i = 0; i < outlen; i++) printf("%02x ", testText[i]);
		printf("\n\n");
	}
	printf("after dec Ghash:\n");
	for (int i = 0; i < AES256gcm_HASHBYTES; i++)
		printf("%02x ", g.dec_tag_result[i]);
	printf("\n\n");

	free(x.stream_key);
	free(g.ad);
}

DEF_CLI(exit, "exit") {
	quitFlag = true;
}

void CLI_BLOCK_INIT() {
	ADD_CLI(manual);
	ADD_CLI(print_test);
	ADD_CLI(Euler_phi_function);
	ADD_CLI(Euler_phi);
	ADD_CLI(AES128_test);
	ADD_CLI(fft_test);
	ADD_CLI(gcm_mode);
	ADD_CLI(exit);
}


