#include "stack.h"

static int StackResize(Stack *stack, size_t nsize);
static void setCanaryPtr(val_t **data);                    
static void setDataPtr(val_t **data);                      

int _StackCtor(Stack *stack, int size, const int src_line, const char *src_file)
{	
	CHECK_FOR_INIT;
#ifdef GREAT_PRINT
	printf("size = %d\n", size);
#endif
#if CANARIES_CHECK == 1
	stack->LCANARY = CANARYVAL;
	stack->RCANARY = CANARYVAL;
	stack->capacity = sizeAlign(sizeof(val_t), size, sizeof(uint64_t))/sizeof(val_t);
#endif
	stack->size = 0;

#if CANARIES_CHECK == 1
	stack->data = (val_t*)calloc(sizeAlign(sizeof(val_t), size, sizeof(uint64_t)) + 2 * sizeof(uint64_t), 1);
#else
	stack->capacity = size;
	stack->data = (val_t*)calloc(sizeof(val_t), stack->capacity);
#endif

	if (stack->data == NULL) {
		EXIT_DUMP;
	}
#if CANARIES_CHECK == 1
	*(uint64_t*)stack->data = CANARYVAL;
	setDataPtr(&stack->data);
	*(uint64_t*)(stack->data + size) = CANARYVAL;
#endif
	SET_HASH;

	ASSERT_OK;
	return 0;
}

int StackDtor(Stack *stack) 
{	
	ASSERT_OK;
	
	memset(stack->data, 0, stack->capacity * sizeof(val_t));

	stack->capacity = 0;
	stack->size     =-1;
#if HASH_CHECK == 1
	stack->hash     = 0;
#endif
#if CANARIES_CHECK == 1 
	setCanaryPtr(&stack->data);
#endif
	free(stack->data);
	stack->data = (val_t*)(POISONED_MEM);

	return NO_ERR;
}

val_t StackPop(Stack *stack)
{	
	POP_ASSERT_OK;
	
	if (StackCheck(stack))
		return NULL;//ERRNUM;
	
	if (stack->size <= 0) {
		ERRNUM = UNDERFLOW_ERR;
		EXIT_DUMP;
	}
	
	val_t val = stack->data[--stack->size];
	stack->data[stack->size] = POISONED_VAL;

	if (stack->size * 4 == stack->capacity && stack->capacity > MINIMUM_STACK_SIZE) {
		SET_HASH;
		if (StackResize(stack, stack->capacity / 2)) 
			return 0;
	}

	SET_HASH;	
	POP_ASSERT_OK;

	return val;
}

val_t StackTop(Stack *stack)
{
	ASSERT_OK;

	return stack->data[stack->size - 1];
}

int StackPush(Stack *stack, val_t val)
{
	ASSERT_OK;
	
	if (StackCheck(stack))
		return ERRNUM;

	if (stack->size >= stack->capacity) {
		ERRNUM = StackResize(stack, 2 * stack->capacity);
		if (ERRNUM != 0)
			return ERRNUM;
	}
	stack->data[stack->size++] = val;
	
	SET_HASH;
	ASSERT_OK;
	return 0;
}

int getStackSize(Stack *stack)
{
	POP_ASSERT_OK;
	return stack->size;
}

val_t *getStackData(Stack *stack)
{
	POP_ASSERT_OK;
	return stack->data;
}

static int StackResize(Stack *stack, size_t nsize)
{
	ASSERT_OK;
#if CANARIES_CHECK == 1
	*(uint64_t*)(stack->data + stack->capacity) = 0; 

	setCanaryPtr(&stack->data);
#endif
	
#if CANARIES_CHECK == 0 
	stack->data = (val_t*)reallocarray(stack->data, nsize, sizeof(val_t));
	memset(stack->data+ stack->size, 0, sizeof(val_t) * (nsize - stack->size));
#else 
	stack->data = (val_t*)reallocarray(stack->data, sizeAlign(sizeof(val_t), nsize,
				sizeof(uint64_t)) + 2 * sizeof(uint64_t), 1);
	memset((char *)(stack->data+ stack->size) + sizeof(uint64_t), 0, sizeof(val_t) * (nsize - stack->size));
#endif


#if CANARIES_CHECK == 1 
	setDataPtr(&stack->data);
		
	*(uint64_t*)(stack->data + nsize) = CANARYVAL;
#endif
	if (stack == NULL)
		return REALL_ERR;
#if CANARIES_CHECK == 1 
	stack->capacity = sizeAlign(sizeof(val_t), nsize, sizeof(uint64_t))/sizeof(val_t);;
#else 
	stack->capacity = nsize;
#endif	
	SET_HASH;
	ASSERT_OK;

	return NO_ERR;
}

int StackSetFileName(Stack *stack, const char *name)
{
#if MULTIPLE_FILES == 1 
	if (name == NULL || stack == NULL)
		return -1;
	else {
		stack->filename = (char *)name;
	}
#endif
	return 0;
}

#define CHECK_STACK(what, code)      if (what) SET_ERR (code)

int StackCheck(Stack *stack)
{
	CHECK_STACK(stack == NULL, 							NULLPTR_STACK);
	CHECK_STACK(stack->data == NULL, 						UNINIT_DATA);
	CHECK_STACK(stack->data == (val_t *)(POISONED_MEM),				POISONED_STACK);
	CHECK_STACK(stack->size > stack->capacity, 					STACK_OVERFLOW);
	CHECK_STACK(stack->size < 0, 						UNDERFLOW_ERR);
#if CANARIES_CHECK == 1	
	CHECK_STACK(stack->LCANARY != CANARYVAL && stack->RCANARY != CANARYVAL, 	INVALID_CANARIES);
	CHECK_STACK(stack->LCANARY != CANARYVAL, 					INVALID_LCANARY);
	CHECK_STACK(stack->RCANARY != CANARYVAL, 					INVALID_RCANARY);
	CHECK_STACK(*(uint64_t*)((char *)stack->data-sizeof(uint64_t)) != CANARYVAL, INVALID_DATA_LCANARY);
	CHECK_STACK(*(uint64_t*)(stack->data + stack->capacity) != CANARYVAL,        INVALID_DATA_RCANARY);
#endif

#if HASH_CHECK == 1
	CHECK_STACK(stack->hash != StackHash(stack),					INVALID_HASH);
#endif	
	return NO_ERR;
}

#undef CHECK_STACK

#define file stdout
#undef file

void _StackDump(Stack *stack, const char *srcfunc, const char *srcfile, const int line) {
#if MULTIPLE_LOGS == 0
	FILE *file = fopen("log.txt", "w");
#else 
	FILE *file = NULL;
	if (stack->filename != NULL)
		file = fopen(stack->filename, "w");
	else 
		file = fopen("log.txt", "w");
#endif
	if (file == NULL) 
		perror("Can't open/create log file\n");

	if (ERRNUM) {
		fprintf(file,"ERROR : %s\n", errmsg(ERRNUM));
	} else {
		goto skip_err_print;
	}

	if (stack == NULL) {
		fclose(file);
		return;
	}
	
	fprintf(file, "Stack<%s>[%p]:",typeid(val_t).name(), stack);	
	if (ERRNUM)
		fprintf(file, "ERROR!");
	else
		fprintf(file, "ok!");

	fprintf(file, "; \"%s\" called from %s at %s (%d)\n",
		       VAR_NAME(stack), srcfunc, srcfile, line);
#if HASH_CHECK == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)
		fprintf(file, "\thash :  %" PRIu32"\n", StackHash(stack));	
#endif 
#if CANARIES_CHECK == 1
	fprintf(file,"\tstructure left canary = %" PRIu64 "\n\tstructure right canary = %" 
			PRIu64 "\n",stack->LCANARY,stack->RCANARY);
#endif	
	fprintf(file, "{\n\tcapasity = %zu;\n\tsize = %d;\n", stack->capacity, stack->size);

#if CANARIES_CHECK == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)	
		fprintf(file,"\tleft data canary[%p] = %" PRIu64 "\n", (char *)stack->data-sizeof(uint64_t),*(uint64_t*)((char *)stack->data-sizeof(uint64_t)));
#endif

	fprintf(file, "\tdata[%p]\n\t{\n", stack->data);
skip_err_print:
	if (stack->data == NULL) {
		fprintf(file, "\t\t[0] = NULL;\n");
	} 
	else if (stack->data == (val_t*)POISONED_MEM) {
		fprintf(file, "\t\t[0] = %d[POISONED];\n", POISONED_MEM);
	}

	else {
		for (int i = 0; i != stack->capacity; i++) {
			fprintf(file,"\t\t*[%d] [%p] = ", i, stack->data + i);
			if (stack->data[i] == POISONED_VAL && i >= stack->size)
				fprintf(file,"POISONED\n");
			else
				printStackVal(stack->data[i], file);
		}
	}
	if (ERRNUM == 0) {
		fclose(file);
		return;
	}

	fprintf(file,"\t}\n");
#if CANARIES_CHECK == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)
		fprintf(file,"\tright data canary[%p] : %" PRIu64 "\n", 
			stack->data + stack->capacity,*(uint64_t*)(stack->data + stack->capacity));
#endif
	fprintf(file,"}\n\n");
	fclose(file);

}

static void setCanaryPtr(val_t **data) 
{
	if (*data != NULL)
		*data = (val_t*)((char*)(*data) - sizeof(uint64_t));
}

static void setDataPtr(val_t **data)
{
	if (*data != NULL)
		*data = (val_t*)((char*)(*data) + sizeof(uint64_t));
}

int sizeAlign(size_t val_s, int numb, int aligment)
{
	int len = val_s * numb;
	if (len % aligment != 0)
		return (len + val_s * (len % aligment));	
	return len;
}

//An algorithm produced by Professor Daniel J. Bernstein

uint32_t djb_hash(const char* data, size_t length)
{
	unsigned int hash = 5381; // MAGIC NUMBER
	
	for (int i = 0; i < length; ++i) {
		hash = (hash << 5) + hash + data[i];
	}

	return hash;
}
#if HASH_CHECK == 1
uint32_t StackHash(Stack *stack)
{
	uint32_t dhash = 0, stackhash = 0;

	dhash = djb_hash((const char *)stack->data, stack->size * sizeof(val_t));

	uint32_t current_hash = stack->hash;	
	stack->hash = dhash;
		
	dhash = djb_hash((const char *)stack, sizeof(stack));

	stack->hash = current_hash;

	return dhash;	
}
#endif
