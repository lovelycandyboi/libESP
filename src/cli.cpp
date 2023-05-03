#include <stdio.h>
#include "../include/cli.h"
#include "../include/util.h"

CLI_BLOCK* CLI_BLOCK_HEAD = NULL;
char VV_stringArgs[10][CLI_ARG_BUFFER_SIZE];
int VV_stringArgsNumb = 0;
bool quitFlag = false;

#ifdef _WIN32

#include <conio.h>
#include <windows.h>
#define STRCPY strcpy_s
#define STRTOK strtok_s

#else

#define STRCPY strcpy
#define STRTOK strtok_r

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
int _getch() {
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

int _putch(int c) {
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

#endif

CMD_NODE* create_CMD_NODE(char data) {
	CMD_NODE* new_CMD_NODE = (CMD_NODE*)malloc(sizeof(CMD_NODE));
	new_CMD_NODE->charData = data;
	new_CMD_NODE->prev = NULL;
	new_CMD_NODE->next = NULL;
	return new_CMD_NODE;
}

void first_insert(CMD_STR* cmdBuf_List, char data) {
	cmdBuf_List->head = create_CMD_NODE(data);
	cmdBuf_List->tail = cmdBuf_List->head;
	cmdBuf_List->cursor = cmdBuf_List->tail;
}
void insert_after(CMD_STR* cmdBuf_List, CMD_NODE* cur_NODE, char data) {
	CMD_NODE* new_CMD_NODE = create_CMD_NODE(data);
	new_CMD_NODE->prev = cur_NODE;
	new_CMD_NODE->next = cur_NODE->next;
	if (cur_NODE->next != NULL) {
		cur_NODE->next->prev = new_CMD_NODE;
	}
	cur_NODE->next = new_CMD_NODE;

	cmdBuf_List->cursor = cmdBuf_List->cursor->next;
	if (cmdBuf_List->cursor == cmdBuf_List->tail->next) {
		cmdBuf_List->tail = cmdBuf_List->tail->next;
	}
}
void insert_before_head(CMD_STR* cmdBuf_List, char data) {
	CMD_NODE* new_CMD_NODE = create_CMD_NODE(data);
	new_CMD_NODE->next = cmdBuf_List->head;
	new_CMD_NODE->prev = cmdBuf_List->head->prev;
	cmdBuf_List->head = new_CMD_NODE;
	cmdBuf_List->head->prev = new_CMD_NODE->prev;
	cmdBuf_List->head->next = new_CMD_NODE->next;
}

void list_insert(CMD_STR* cmdBuf_List, CMD_NODE* cur_NODE, char data) {
	if (cmdBuf_List->head == NULL)	first_insert(cmdBuf_List, data);	
	else							insert_after(cmdBuf_List, cur_NODE, data);
}

void delete_CMD_NODE(CMD_STR* cmdBuf_List, CMD_NODE* DEL_NODE) {
	if (cmdBuf_List->head == NULL || DEL_NODE == NULL)  return;
	if (cmdBuf_List->head == DEL_NODE)					cmdBuf_List->head = DEL_NODE->next;
	if (DEL_NODE->next != NULL)							DEL_NODE->next->prev = DEL_NODE->prev;
	if (DEL_NODE->prev != NULL)							DEL_NODE->prev->next = DEL_NODE->next;
	if (DEL_NODE == cmdBuf_List->tail)					cmdBuf_List->tail = DEL_NODE->prev;
	free(DEL_NODE);
}

int  tab_strcmp(char* curCmd, char* inputCmd) {

	char temp_curCmd[CLI_ARG_BUFFER_SIZE] = { 0, };
	char temp_inputCmd[CLI_ARG_BUFFER_SIZE] = { 0, };

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
bool command_cmp(char* curCmd, char* inputCmd) {
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

		curCmdIter   = STRTOK(NULL, " ", &curContext);
		inputCmdIter = STRTOK(NULL, " ", &inputContext);
	}

	if (curCmdIter != NULL || inputCmdIter != NULL)
		isMatched = false;
	
	return isMatched;
}

void input_cmd(char* input_cmdBuf) {
	CMD_STR* cmdBuf_List = (CMD_STR*)malloc(sizeof(CMD_STR));
	cmdBuf_List->init();
	char tempChar;
	int cmdBufIdx = 0;
	do {
		tempChar = _getch();
		switch (tempChar) {
		case END_KEY:
			break;
		case TAB_KEY:
		{
			cmdBufIdx = 0;
			for (CMD_NODE* cmdIter = cmdBuf_List->head; cmdIter != NULL; cmdIter = cmdIter->next) {
				input_cmdBuf[cmdBufIdx++] = cmdIter->charData;
			}
			input_cmdBuf[cmdBufIdx] = '\0';
			for (CLI_BLOCK* blockIter = CLI_BLOCK_HEAD; blockIter != NULL; blockIter = blockIter->next) {
				int tabCmpIdx = tab_strcmp(blockIter->command, input_cmdBuf);
				if (tabCmpIdx != -1) {
					if (cmdBufIdx > 0 && input_cmdBuf[cmdBufIdx - 1] != ' ' && tabCmpIdx > 0 && blockIter->command[tabCmpIdx - 1] == ' ') {
						_putch(' ');
						list_insert(cmdBuf_List, cmdBuf_List->cursor, ' ');
					}
					for (int autoMakeIdx = tabCmpIdx; autoMakeIdx < strlen(blockIter->command) && blockIter->command[autoMakeIdx] != ' '; autoMakeIdx++) {
						_putch(blockIter->command[autoMakeIdx]);
						list_insert(cmdBuf_List, cmdBuf_List->cursor, blockIter->command[autoMakeIdx]);
					}
					break;
				}
			}
			break;
		}
		case BAKC_SPACE_KEY:
			if (cmdBuf_List->cursor != NULL) {
				CMD_NODE* toBeDeleted = cmdBuf_List->cursor;
				cmdBuf_List->cursor = cmdBuf_List->cursor->prev;
				delete_CMD_NODE(cmdBuf_List, toBeDeleted);
				
				_putch('\b');
				_putch(' ');
				_putch('\b');
				
				int cursorShift_cnt = 0;
				if (cmdBuf_List->cursor != NULL) {
					for (CMD_NODE* cmdIter = cmdBuf_List->cursor->next; cmdIter != NULL; cmdIter = cmdIter->next) {
						_putch(cmdIter->charData);
						cursorShift_cnt++;
					}
					printf(" \b \b");
					if (cursorShift_cnt > 0) {
						printf("\033[%dD", cursorShift_cnt);
					}
				}
				if (cmdBuf_List->cursor == NULL && cmdBuf_List->head != NULL) {
					for (CMD_NODE* cmdIter = cmdBuf_List->head; cmdIter != NULL; cmdIter = cmdIter->next) {
						_putch(cmdIter->charData);
						cursorShift_cnt++;
					}
					printf(" \b \b");
					if (cursorShift_cnt > 0) {
						printf("\033[%dD", cursorShift_cnt);
					}
				}
			}
			break;
		case ARROW_ENTRY:
			tempChar = _getch();
#ifndef _WIN32
			tempChar = _getch();
#endif
			if (tempChar == LEFT_ARROW_KEY) {
				if (cmdBuf_List->cursor != NULL && cmdBuf_List->cursor->prev != NULL) {
					cmdBuf_List->cursor = cmdBuf_List->cursor->prev;
					printf("\033[1D");
				}
				else if (cmdBuf_List->cursor == cmdBuf_List->head) {
					cmdBuf_List->cursor = NULL;
					printf("\033[1D");
				}
			}
			else if (tempChar == RIGHT_ARROW_KEY) {
				if (cmdBuf_List->cursor != NULL && cmdBuf_List->cursor->next != NULL) {
					cmdBuf_List->cursor = cmdBuf_List->cursor->next;
					printf("\033[1C");
				}
				else if (cmdBuf_List->cursor == NULL && cmdBuf_List->head != NULL) {
					cmdBuf_List->cursor = cmdBuf_List->head;
					printf("\033[1C");
				}
			}
			break;
		default:
			if (cmdBuf_List->cursor == NULL && cmdBuf_List->head != NULL) {
				insert_before_head(cmdBuf_List, tempChar);
				cmdBuf_List->cursor = cmdBuf_List->head;
			}
			else list_insert(cmdBuf_List, cmdBuf_List->cursor, tempChar);
			int cursurShift_cnt = 0;
			for (CMD_NODE* cmdIter = cmdBuf_List->cursor; cmdIter != NULL; cmdIter = cmdIter->next) {
				_putch(cmdIter->charData);
				cursurShift_cnt++;
			}
			if (cursurShift_cnt > 1) {
				printf("\033[%dD", cursurShift_cnt - 1);
			}
			break;

		}
	} while (tempChar != END_KEY);

	printf("\n");

	cmdBufIdx = 0;
	for (CMD_NODE* cmdIter = cmdBuf_List->head; cmdIter != NULL; cmdIter = cmdIter->next) {
		input_cmdBuf[cmdBufIdx++] = cmdIter->charData;
	}
	input_cmdBuf[cmdBufIdx] = 0;
}

bool TERMINAL_CONTROL_CLI() {
	char input_cmdBuf[CLI_ARG_BUFFER_SIZE] = { 0, };
	std::cout << ">>";
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
