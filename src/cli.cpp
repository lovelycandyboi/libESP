#include <string>
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "../include/cli.h"
#include "../include/util.h"

#define CLI_ARG_BUFFER_SIZE 100

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
	strcpy_s(temp_curCmd, curCmd);
	strcpy_s(temp_inputCmd, inputCmd);

	char* curContext   = NULL;
	char* inputContext = NULL;
	char* curCmdIter   = strtok_s(temp_curCmd, " ", &curContext);
	char* inputCmdIter = strtok_s(temp_inputCmd, " ", &inputContext);

	bool isMatched = true;

	while (curCmdIter != NULL && inputCmdIter != NULL)
	{
		if (strcmp(curCmdIter, inputCmdIter)) {
			if (strcmp(curCmdIter, "VV")) { isMatched = false; break; }
			else VV_stringBuf[VV_stringBufIdx++] = inputCmdIter;
		}

		curCmdIter	 = strtok_s(NULL, " ", &curContext);
		inputCmdIter = strtok_s(NULL, " ", &inputContext);
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
	fgets(commandBuffer, CLI_ARG_BUFFER_SIZE, stdin);
	commandBuffer[strlen(commandBuffer) - 1] = NULL;
	
	for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
		if (command_cmp(blockIter->command, commandBuffer)) {
			blockIter->cli_function();
			return quitFlag;
		}
	}

	printf("no matched command\n");
	return quitFlag;
}