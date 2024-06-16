#include <stdio.h>

typedef int (*PFUN)();

void fun(PFUN pfun)
{
	printf("[%d]\n",pfun());
}

int returnNum()
{
	return 250;
}

void main()
{
	fun(returnNum);
	return;
}
