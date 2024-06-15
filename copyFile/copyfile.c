#include <stdio.h>
#include <string.h>

void main(int argc,char* argv[])
{
	char total[10240]={0};

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
		strcat(total,buff);
	}

	fputs(total,fdest);

	fclose(fsrc);
        fclose(fdest);

	return;
}
