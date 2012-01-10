#ifndef Guard_Utility_h

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <ctype.h>
#define PRIVATE static
#define PUBLIC 

typedef struct Pair_
{
	void *first;
	void *second;
} Pair;
typedef struct Stack_{
	SIZE_T size;
	DOUBLE value[256];	
} Stack;
void push(Stack *stack, DOUBLE value)
{
	if(stack->size == 256)
		fputs("Stack overflow\n",stderr);
	stack->value[stack->size++] = value;
}
DOUBLE pop(Stack *stack)
{
	if(stack->size == 0)
		fputs("Stack underflow\n",stderr);
	return stack->value[--stack->size];
}
void Free_Pair(Pair *pair)
{
	free(pair->first);
	free(pair->second);
	free(pair);
}
Pair *Make_Pair(void *f, void *s)
{
	Pair *result;
	result = malloc(sizeof(Pair));
	if(result == NULL)
		return NULL;
	result->first = malloc(sizeof(double));
	result->second = malloc(sizeof(double));
	if(f == NULL || s == NULL)
	{
		Free_Pair(result);
		return NULL;
	}
	memcpy(result->first,f,sizeof(double));
	memcpy(result->second,s,sizeof(double));
	return result;
}


#endif // Guard_Utility_h