#ifndef DEFINES_H
#define DEFINES_H
/* 
 * DEBUG = 0 - RELEASE mode enabled 
 * DEBUG = 1 - full DEBUG mode enabled
 * DEBUG = 2 - DEBUG mode(no asserts)
 */

#define DEBUG 1
/*
 * Set MULTIPLE_LOGS to 0 if you are using only one stack, one log file will be generated.
 * Set MULTIPLE_LOGS to 1 if you use multiple stacks, for each stack you have to point out log file name.
 *
 * RECOMMENDED: use only one log `file.
 */
#define MULTIPLE_LOGS 0
/*
 * Set HASH_CHECK to 1 to enable hash protection
 */

#define HASH_CHECK 1
/*
 * Set CANARIES_CHECK to 1 to enable canaries protection
 */

#define CANARIES_CHECK 1

/*
 * list validate tree intehry
 */
#define VALIDATE_TREE_INTEGRY

/*
 * print tree info while checking
 */

//#define TREE_CHECK_DUMP

/*
 * print node paret in dump
 */
//#define PARENT_PRINT

#endif
