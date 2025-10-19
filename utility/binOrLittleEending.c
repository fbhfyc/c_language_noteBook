#include <stdio.h>

int main()
{
	int num = 1;
	if ( *(char*)&num ) {
		printf("little ending\n");
	} else {
		printf("big ending\n");
	}
	return 1;
}
