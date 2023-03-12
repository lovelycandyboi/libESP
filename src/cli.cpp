#include <stdio.h>
#include "../include/cli.h"
#include "../include/util.h"


CLI_BLOCK* CLI_BLOCK_HEAD = NULL;
char VV_stringArgs[10][CLI_ARG_BUFFER_SIZE];
int VV_stringArgsNumb = 0;
bool quitFlag = false;

#ifdef _WIN32
#include <conio.h>
#define STRCPY strcpy_s
#define STRTOK strtok_s

#else
#define STRCPY strcpy
#define STRTOK strtok_r
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
int getch() {
	int c;
	struct termios oldattr, newattr;

	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	newattr.c_cc[VMIN] = 1;
	newattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	c = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	return c;
}

int putch(int c) {
	struct termios o, n;

	ioctl(1, TCGETA, &o);
	ioctl(1, TCGETA, &n);

	n.c_lflag &= ~(ICANON | ECHO);
	ioctl(1, TCSETA, &n);

	if (write(1, &c, 1) < 0)
		return EOF;

	ioctl(1, TCSETA, &o);

	return c;
}

void input_cmd(char* input_cmdBuf) {
	int cmdBufIdx = 0;
	char tempChar;
	do {
		tempChar = getch();

		if (tempChar == '\t') {
			for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
				int tabCmpIdx = tab_strcmp(blockIter->command, input_cmdBuf);
				if (tabCmpIdx != -1) {
					if (cmdBufIdx > 0 && input_cmdBuf[cmdBufIdx - 1] != ' ' && blockIter->command[tabCmpIdx - 1] == ' ') {
						putch(' ');
						input_cmdBuf[cmdBufIdx++] = ' ';
					}

					for (int i = tabCmpIdx; i < strlen(blockIter->command); i++) {
						if (blockIter->command[i] == ' ') break;
						putch(blockIter->command[i]);
						input_cmdBuf[cmdBufIdx++] = blockIter->command[i];
					}

					break;
				}
			}
		}
		else if (tempChar == '\b' || tempChar == 0x7f) {
			if (cmdBufIdx > 0) {
				input_cmdBuf[--cmdBufIdx] = '\0';
				putch('\b');
				putch(' ');
				putch('\b');
			}
		}
		else if (0 <= tempChar && tempChar <= 127) {
			input_cmdBuf[cmdBufIdx++] = tempChar;
			putch(tempChar);
		}

	} while (tempChar != '\n');

	input_cmdBuf[cmdBufIdx - 1] = '\0';
	printf("\n");
}
#endif

bool command_cmp(char* curCmd, char* inputCmd){
	VV_stringArgsNumb = 0;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 100; j++)
			VV_stringArgs[i][j] = NULL;

	char temp_curCmd[CLI_ARG_BUFFER_SIZE];
	char temp_inputCmd[CLI_ARG_BUFFER_SIZE];
	STRCPY(temp_curCmd, curCmd);
	STRCPY(temp_inputCmd, inputCmd);

	char* curContext = NULL;
	char* inputContext = NULL;
	char* curCmdIter = STRTOK(temp_curCmd, " ", &curContext);
	char* inputCmdIter = STRTOK(temp_inputCmd, " ", &inputContext);

	bool isMatched = true;

	while (curCmdIter != NULL && inputCmdIter != NULL) {
		if (strcmp(curCmdIter, inputCmdIter)) {
			if (strcmp(curCmdIter, "VV")) { isMatched = false; break; }
			else {
				STRCPY(VV_stringArgs[VV_stringArgsNumb], inputCmdIter);
				VV_stringArgsNumb++;
			}
		}

		curCmdIter = STRTOK(NULL, " ", &curContext);
		inputCmdIter = STRTOK(NULL, " ", &inputContext);
	}

	if (curCmdIter != NULL || inputCmdIter != NULL)	isMatched = false;
	return isMatched;
}

int  tab_strcmp(char* curCmd, char* inputCmd) {
	
	char temp_curCmd[CLI_ARG_BUFFER_SIZE] = {0,};
	char temp_inputCmd[CLI_ARG_BUFFER_SIZE] = {0,};
	
	STRCPY(temp_curCmd, curCmd);
	STRCPY(temp_inputCmd, inputCmd);

	char* curContext = NULL;
	char* inputContext = NULL;
	char* curCmdIter = STRTOK(temp_curCmd, " ", &curContext);
	char* inputCmdIter = STRTOK(temp_inputCmd, " ", &inputContext);

	int curCmdIdx = 0;
	int tokcnt = 0;
	while (curCmdIter != NULL && inputCmdIter != NULL) {
		if (strcmp(curCmdIter, inputCmdIter) && strcmp(curCmdIter, "VV")) {
			if (strncmp(curCmdIter, inputCmdIter, strlen(inputCmdIter)) == 0)
				return curCmdIdx + strlen(inputCmdIter) + tokcnt;
			else return -1;
		}
		else curCmdIdx += strlen(curCmdIter);
		curCmdIter = STRTOK(NULL, " ", &curContext);
		inputCmdIter = STRTOK(NULL, " ", &inputContext);
		tokcnt++;
	}

	return curCmdIdx + tokcnt;
	
}

void input_cmd(char* input_cmdBuf) {
	int cmdBufIdx = 0;
	char tempChar;

	do {
		tempChar = _getch();

		if (tempChar == '\t') {
			for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
				int tabCmpIdx = tab_strcmp(blockIter->command, input_cmdBuf);
				if (tabCmpIdx != -1) {
					if (cmdBufIdx > 0 && input_cmdBuf[cmdBufIdx - 1] != ' ' && blockIter->command[tabCmpIdx - 1] == ' ') {
						_putch(' ');
						input_cmdBuf[cmdBufIdx++] = ' ';
					}

					for (int i = tabCmpIdx; i < strlen(blockIter->command); i++) {
						if (blockIter->command[i] == ' ') break;
						_putch(blockIter->command[i]);
						input_cmdBuf[cmdBufIdx++] = blockIter->command[i];
					}

					break;
				}
			}
		}
		else if (tempChar == '\b') {
			if (cmdBufIdx > 0) {
				input_cmdBuf[--cmdBufIdx] = '\0';
				_putch('\b');
				_putch(' ');
				_putch('\b');
			}
		}
		else if (0 <= tempChar && tempChar <= 127) {
			input_cmdBuf[cmdBufIdx++] = tempChar;
			_putch(tempChar);
		}

	} while (tempChar != '\r');

	input_cmdBuf[cmdBufIdx - 1] = '\0';
	printf("\n");
}

bool TERMINAL_CONTROL_CLI() {
	char input_cmdBuf[CLI_ARG_BUFFER_SIZE] = { 0, };
	std::cout << "esp cli >> ";
	input_cmd(input_cmdBuf);
	for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
		if (command_cmp(blockIter->command, input_cmdBuf)) {
			blockIter->cli_function();
			return quitFlag;
		}
	}

	printf("no matched command\n");
	return quitFlag;
}
