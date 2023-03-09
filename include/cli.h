#ifndef __CLI_H___
#define __CLI_H___

#include <stdint.h>

#define CLI_BLOCK_ADD(head, block)														\
		do{																				\
			if(head == NULL){															\
				(head) = (block);														\
				(head)->next = NULL;													\
			}																			\
			else{																		\
				(block)->next = (head)->next;											\
				(head)->next = (block);													\
			}																			\
		}while(0)

#define DEF_CLI(function, commandStr) 													\
		CLI_BLOCK cli_##function##_block = { NULL, NULL, commandStr};					\
		void cli_##function()											

#define ADD_CLI(function)																\
		do{																				\
			cli_##function##_block.cli_function = cli_##function;						\
			CLI_BLOCK_ADD(CLI_BLOCK_HEAD, &cli_##function##_block);						\
		}while(0)

#define CLI_ARG_BUFFER_SIZE 100

bool TERMINAL_CONTROL_CLI();
void CLI_BLOCK_INIT();

bool command_cmp(char* curCmd, char* inputCmd);
int  tab_strcmp(char* curCmd, char* inputCmd);
void input_cmd(char* input_cmdBuf);

void hexStr2int(char* src, int* dst);
void decStr2int(char* src, int* dst);

typedef struct _CLI_BLOCK {
	struct _CLI_BLOCK* next;
	void (*cli_function)(void);
	char command[100];
}CLI_BLOCK;


#endif
