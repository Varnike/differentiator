#include "printval.h"

void printStackVal(double value, FILE *file)
{
	fprintf(file,"%f\n", value);
}
void printStackVal(char value, FILE *file)
{
	fprintf(file,"%d\n", (int)value);
}
void printStackVal(int value, FILE *file)
{
	fprintf(file,"%d\n", value);
}
void printStackVal(long long value, FILE *file)
{
	fprintf(file,"%lld\n", value);
}
void printStackVal(size_t value, FILE *file)
{
	fprintf(file, "%zu\n", value);
}

void printStackVal(void *value, FILE *file)
{
	fprintf(file, "%p\n", value);
}
