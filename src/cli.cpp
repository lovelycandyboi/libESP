#include <string>
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "../include/cli.h"
#include "../include/util.h"

#define CLI_ARG_BUFFER_SIZE 100

#ifdef _WIN32
	#include <conio.h>
	#define STRCPY strcpy_s
	#define STRTOK strtok_s	


	void input_cmd(char* input_cmdBuf) {
	int cmdBuf_idx = 0;
	char tempChar;
	do {
		tempChar = _getch();
		if (cmdBuf_idx <= 0) {
			cmdBuf_idx = 0;
			input_cmdBuf[0] = '\0';
		}
		if (tempChar == '\t') {
			bool stopIterFlag = false;
			for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {

				for (int tabCmpIdx = 0; tabCmpIdx < strlen(blockIter->command); tabCmpIdx++) {
					if (input_cmdBuf[tabCmpIdx] != blockIter->command[tabCmpIdx]) {
						if (tabCmpIdx < cmdBuf_idx)	break;
						else {
							stopIterFlag = true;
							_putch(blockIter->command[tabCmpIdx]);
							input_cmdBuf[cmdBuf_idx++] = blockIter->command[tabCmpIdx];
						}
					}
				}

				if (stopIterFlag) break;
			}

		}
		else if (tempChar == '\b' && cmdBuf_idx > 0) {
			input_cmdBuf[--cmdBuf_idx] = '\0';
			_putch('\b');
			_putch(' ');
			_putch('\b');
		}
		else {
			input_cmdBuf[cmdBuf_idx++] = tempChar;
			_putch(tempChar);
		}
	} while (tempChar != '\r');

	input_cmdBuf[cmdBuf_idx - 1] = '\0';

}
#else
	#define STRCPY strcpy
	#define STRTOK strtok_r
#endif

extern CLI_BLOCK* CLI_BLOCK_HEAD;
char* VV_stringBuf[10];
int VV_stringBufIdx = 0;
bool quitFlag = false;

bool command_cmp(char* curCmd, char* inputCmd)
{
	VV_stringBufIdx = 0;
	for (int i = 0; i < 10; i++) VV_stringBuf[i] = NULL;

	char temp_curCmd[CLI_ARG_BUFFER_SIZE];
	char temp_inputCmd[CLI_ARG_BUFFER_SIZE];
	STRCPY(temp_curCmd, curCmd);
	STRCPY(temp_inputCmd, inputCmd);

	char* curContext   = NULL;
	char* inputContext = NULL;
	char* curCmdIter   = STRTOK(temp_curCmd, " ", &curContext);
	char* inputCmdIter = STRTOK(temp_inputCmd, " ", &inputContext);

	bool isMatched = true;

	while (curCmdIter != NULL && inputCmdIter != NULL)
	{
		if (strcmp(curCmdIter, inputCmdIter)) {
			if (strcmp(curCmdIter, "VV")) { isMatched = false; break; }
			else VV_stringBuf[VV_stringBufIdx++] = inputCmdIter;
		}

		curCmdIter	 = STRTOK(NULL, " ", &curContext);
		inputCmdIter = STRTOK(NULL, " ", &inputContext);
	}

	if (curCmdIter != NULL || inputCmdIter != NULL)	isMatched = false;

	return isMatched;
}

void CLI_GET_ARG(char* dest, int arg_index) {
	dest = VV_stringBuf[arg_index];
	printf("%s %s\n", VV_stringBuf[0],dest);
}


bool TERMINAL_CONTROL_CLI() {
	char commandBuffer[CLI_ARG_BUFFER_SIZE];
	std::cout << "esp cli >> ";
#ifdef _WIN32
	input_cmd(commandBuffer);
#else
	fgets(commandBuffer, CLI_ARG_BUFFER_SIZE, stdin);
	commandBuffer[strlen(commandBuffer) - 1] = NULL;
#endif
	for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
		if (command_cmp(blockIter->command, commandBuffer)) {
			blockIter->cli_function();
			return quitFlag;
		}
	}

	printf("no matched command\n");
	return quitFlag;
}