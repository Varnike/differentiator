#include "error.h"

int ERRNUM = NO_ERR;

const char *errmsg(int errt)
{
        const char *messages[] = {
                "Success",
                "Unknown error type!",
                "Calloc error!",
                "An error occured while reading from file!",
                "An error occured while writing into file!",
		"Structure pointer error!",
		"Buffer error!",
		"An error occured while trying to get file size!",
		"An error occured while closing file!",
		"An error occured while opening file!",
		"Realloc error!",
		"Underflow error!",
		"Stack structure was destroyed, but not initialized!",
		"Stack NULL pointer!",
		"Uninizialized stack data!",
		"Stack overflow!",
		"Invalid stack for StackCtor!",
		"Invalid structure left canary!",
		"Invalid structure right canary!",
		"Both canaries are invalid",
		"Invalid data right canary!",
		"Invalid data left canary!",
		"Invalid hash, data corrupted!"
        };

	const char *proc_messages[] = {
		"Assembler: syntax error!",
		"Unknown command error!",
		"Unknown value!",
		"Wrong input file version!",
		"Invalid file signature!",
		"Invalid binary data!",
		"Error occured while reading header!",
		"Invalid input data for assembler!",
		"Labels limit exceeded!",
		"Syntax error: several labels of same name!"
	};
	
	const char *list_messages[] = {
		"Invalid list address error!",
		"List bad head node error!",
		"List no free space error!",
		"List node not found!",
		"Lisr is broken!",
		"List bad tail node error!",
		"List node bad next error!",
		"List node bad prev error!",
		"List bad node connection!",
		"List bad free addres!",
		"List free list breoken!",
		"List invalid free list node!",
		"Bad list for ctor!"
	};
	
	const char *tree_messages[] = {
		"Tree broken: loop found!",
		"Tree busy parent for insertion(not NULL pointer)!",
		"Tree NULL data pointer error!",
		"Tree only one child error!",
		"Tree same left & right child error!",
		"Tree NULL node pointer error!",	
		"Tree bad root node for ctor error!",
		"Akinator syntax error!",
		"Akinator bad init string!",
		"Akinator NULL argument",
		"Akinator buffer overflow"
	};

	const char *diff_messages[] = {
		"Differentiator unknown node type!",
		"Differentiator syntax error!",
		"Differentiator unknown operation type!"
	};

	switch (errt/100) {
	case 0:
		return messages[errt];
		break;
	case 1:
		return proc_messages[errt - 100];
		break;
	case 2:
		return list_messages[errt - 200];
		break;
	case 3:
		return tree_messages[errt - 300];
	case 4:
		return diff_messages[errt - 400];
		break;
	default:
		return messages[UNKNOWN_ERR];
		break;
	}
#if 0
        if (errt >= 0   && errt <= ERRS_CNT)
                return messages[errt];

	if (errt >= 100 && errt < PROC_ERRS + 100)
		return proc_messages[errt - 100];
#endif
        return messages[UNKNOWN_ERR];
}

