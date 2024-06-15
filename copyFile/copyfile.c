#include <stdio.h>
#include <string.h>

void main(int argc,char* argv[])
{

	if (argc != 3) {
		printf("useg err");
		return;
	}

 	char buff[1024]={0};

	FILE* fsrc = fopen(argv[1],"r");
	if (fsrc == NULL) {
		printf("file open failed");
		return;
	}

	FILE* fdest = fopen(argv[2],"w");
	if(fdest == NULL) {
		printf("file open failed");
		return;
	}

	while (fgets(buff,1024,fsrc) != NULL) {
	        fputs(buff,fdest);
	}


	fclose(fsrc);
        fclose(fdest);

	return;
}
