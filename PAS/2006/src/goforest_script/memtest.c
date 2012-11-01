#include <stdio.h>
#include <unistd.h>

main()
{
	void *tmp;
	char command[1024];
	unsigned int size;
	void *res;
	int ires;

	int i = 0;

	for(i = 0; i < 20; i++) 
	{
		tmp = (char *)malloc((i+1)*10*1024*1024);
		free(tmp);
//		ires = brk(tmp);
//		sprintf(command, "date >> top1.log;top -n | grep pasgw >> top1.log; echo >> top1.log");
//		system(command);
		fprintf(stderr,"i = %d\n", i+1);
		sleep(2);
	}

	sleep(10);

	for(i = 0; i < 20; i++) 
	{
		tmp = (char *)malloc((i+1)*10*1024*1024);
		free(tmp);
		fprintf(stderr,"i = %d\n", i+1);
		sleep(2);
	}

	sleep(7200);

	exit(0);
}
