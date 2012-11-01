#include <stdio.h>
#include <unistd.h>

main()
{
	char *tmp;
	char command[1024];
	unsigned int size;
	void *res;

	int i = 0;

	sprintf(command, "mv top1.log /tmp/tmp1.log");
	fprintf(stderr,"\"%s\" is ready", command);
	system(command);
	fprintf(stderr,"\"%s\" is complete", command);

	exit(0);
}
