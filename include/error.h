/**
 * @file
 *
 * @brief Set of functions for error handling.
 *
 * @author Varnike
 *
 * @date 21 Sep, 1:40 UTC+3.
 */


#ifndef ERROR_H
#define ERROR_H

#include <cstdio>
/**
 * @brief Stores most recent error code.
 *
 * Heavily lightweight errno analog.
 */

extern int ERRNUM;

const int ERRS_CNT = 23;

/*
 * Stack and most common errors.
 */
enum errtype {
	NO_ERR	        = 0,
	UNKNOWN_ERR     = 1,
	CALLOC_ERR      = 2,
	READ_ERR        = 3,
	WRITE_ERR       = 4,
	STRUCT_PTR_ERR  = 5,
	BUFF_PTR_ERR    = 6,
	FILE_SIZE_ERR   = 7,
	FCLOSE_ERR      = 8,
	FOPEN_ERR       = 9,
	REALL_ERR       = 10,
	UNDERFLOW_ERR   = 11,
	POISONED_STACK  = 12,
	NULLPTR_STACK   = 13,
	UNINIT_DATA     = 14,
	STACK_OVERFLOW  = 15,
	INVALID_STACK   = 16,
	INVALID_LCANARY = 17,
	INVALID_RCANARY = 18,
	INVALID_CANARIES= 19,
	INVALID_DATA_RCANARY   = 20,
       	INVALID_DATA_LCANARY   = 21,
	INVALID_HASH	       = 22
};

const int PROC_ERRS = 10; 

/*
 * CPU and assembler errors.
 */
enum assert_errs {
	SYNTAX_ERR	  = 100,
	UNKNOWN_CMD_ERR	  = 101,
	UNKNOWN_VAL_ERR   = 102,
	WRONG_VERSION_ERR = 103,
	INVALID_SIGN_ERR  = 104,
	INVALID_BIN_DATA  = 105,
	HDR_READ_ERR	  = 106,
	INVALID_INPUT_VAL = 107,
	LABEL_CNT_ERR     = 108,
	LABEL_NAME_CNT_ERR= 109
};

const int LIST_ERRS = 13;
/*
 * LIST errors.
 */

enum list_errs {
	LIST_INV_POS_ERR      = 200,
	LIST_BAD_HEAD_ERR     = 201,
	LIST_NO_FREE_SPACE    = 202,
	LIST_NODE_NOT_FOUND   = 203,
	LIST_BROKEN           = 204,
	LIST_BAD_TAIL_ERR     = 205,
	LIST_NODE_BAD_NEXT    = 206,
	LIST_NODE_BAD_PREV    = 207,
	LIST_CONNECTION_ERR   = 208,
	LIST_BAD_FREE_ERR     = 209,
	LIST_FREE_LIST_BROKEN = 210,
	LIST_BAD_FREE_NODE    = 211,
	LIST_CTOR_ERR         = 212,
	LIST_LOGIC_ADDR_ERR   = 213
};

const int TREE_ERRS = 7;
/*
 * TREE errors
 */

enum tree_errs {
	TREE_LOOP_FOUND     = 300,
	TREE_BUSY_PARENT    = 301,
	TREE_NULL_DATA      = 302,
	TREE_ONE_CHILD      = 303,
	TREE_SAME_CHILD     = 304,
	TREE_NULL_NODE      = 305,
	TREE_BAD_CTOR_ROOT  = 306,
	AKINATOR_SYNTAX_ERR = 307,
	AKINATOR_BAD_INIT_STR = 308,
	AKINATOR_NULL_ARG     = 309,
	AKINATOR_BUFF_OVERFLOW= 310
};
/*
 * Returns a pointer to the textual description of the code errtype.
 *
 * Uses error enumeration to determine error string.
 * @param errt error code.
 *
 * @return Pointer to a null-terminated byte string corresponding to the errtype code.
 */

const char *errmsg(int errt);

#endif //ERROR_H
