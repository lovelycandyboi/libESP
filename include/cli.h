#ifndef __CLI_H___
#define __CLI_H___

#include <stdint.h>
#ifdef _WIN32
#define END_KEY			'\r'
#define TAB_KEY			'\t'
#define BAKC_SPACE_KEY	'\b'
#define ARROW_ENTRY		-32
#define LEFT_ARROW_KEY	 75
#define RIGHT_ARROW_KEY  77
#else
#define END_KEY			'\n'
#define TAB_KEY			'\t'
#define BAKC_SPACE_KEY	'\177'
#define ARROW_ENTRY		 27
#define LEFT_ARROW_KEY	 68
#define RIGHT_ARROW_KEY  67
#endif
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

typedef struct CLI_BLOCK {
	struct CLI_BLOCK* next;
	void (*cli_function)(void);
	char command[100];
}CLI_BLOCK;
typedef struct CMD_NODE {
	char   charData;
	struct CMD_NODE* prev;
	struct CMD_NODE* next;
}CMD_NODE;
typedef struct CMD_STR {
	CMD_NODE* head;
	CMD_NODE* cursor;
	CMD_NODE* tail;
	void init() {
		this->head = NULL;
		this->cursor = NULL;
		this->tail = NULL;
	}
}CMD_STR;

CMD_NODE* create_CMD_NODE(char data);
void first_insert(CMD_STR* cmdBuf_List, char data);
void insert_after(CMD_STR* cmdBuf_List, CMD_NODE* cur_NODE, char data);
void insert_before_head(CMD_STR* cmdBuf_List, char data);
void list_insert(CMD_STR* cmdBuf_List, CMD_NODE* cur_NODE, char data);
void delete_CMD_NODE(CMD_STR* cmdBuf_List, CMD_NODE* DEL_NODE);


#endif