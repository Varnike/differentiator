#ifndef _PRINTVAL_H
#define _PRINTVAL_H

#include <stdio.h>

void printStackVal(double value, FILE *file);      
void printStackVal(char value, FILE *file);        
void printStackVal(int value, FILE *file);         
void printStackVal(long long value, FILE *file);        
void printStackVal(size_t value, FILE *file);      
void printStackVal(void *value, FILE *file);
#endif // _PRINTVAL_H
