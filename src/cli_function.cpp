#include <stdio.h>
#include <stdlib.h>
#include "../include/cli.h"
#include "../crypto/aes.h"

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
	for (int i = 0; i < 16; i++)	printf("%c", plainText[i]);	printf("\n");
}

/*
DEF_CLI(AES128_test_cmd, "aes VV with VV") {

	const char* textFileName = VV_stringArgs[0];
	const char* keyFileName  = VV_stringArgs[1];

	byte plainTextBlock[4][4] = { 0 };
	byte aesKeyBlock[4][4] = { 0 };

	std::ifstream textInputFile(textFileName);	std::ifstream keyInputFile(keyFileName);
	if (!textInputFile.is_open())	{ printf("%s is not exist!\n", textFileName);	return;	}
	if (!keyInputFile.is_open())	{ printf("%s is not exist!\n", keyFileName);	return;	}
	for (int r = 0; r < 4; r++)	for (int c = 0; c < 4; c++)	textInputFile >> plainTextBlock[r][c];
	for (int r = 0; r < 4; r++)	for (int c = 0; c < 4; c++)	keyInputFile >> aesKeyBlock[r][c];
	textInputFile.close(); keyInputFile.close();

	printf("before aes128 encrypt->");
	for (int r = 0; r < 4; r++)	for (int c = 0; c < 4; c++)	printf("%c", plainTextBlock[r][c]);	printf("\n");

	AES_encryption(plainTextBlock, aesKeyBlock);
	printf("aes128 encrypt result->");
	for (int r = 0; r < 4; r++)	for (int c = 0; c < 4; c++)	printf("%c", plainTextBlock[r][c]);	printf("\n");

	AES_decryption(plainTextBlock, aesKeyBlock);
	printf("aes128 decrypt result->");
	for (int r = 0; r < 4; r++)	for (int c = 0; c < 4; c++)	printf("%c", plainTextBlock[r][c]);	printf("\n");

}
*/

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

DEF_CLI(exit, "exit") {
	quitFlag = true;
}

void CLI_BLOCK_INIT() {
	ADD_CLI(manual);
	ADD_CLI(print_test);
	ADD_CLI(Euler_phi_function);
	ADD_CLI(Euler_phi);
	ADD_CLI(AES128_test);
	//ADD_CLI(AES128_test_cmd);
	ADD_CLI(fft_test);
	ADD_CLI(exit);
}


