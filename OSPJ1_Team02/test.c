#include <sys/syscall.h>
#include <syscall.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	printf("Show th IDs and names.\n");
	syscall(337);
	printf("Insert 2 numbers to multiply.\n");
	long a,b;	
	scanf("%ld%ld",&a,&b);
	printf("%ld\n",syscall(338,a,b));
	printf("Insert 2 numbers to find the min.\n");
	scanf("%ld%ld",&a,&b);
	printf("%ld\n",syscall(339,a,b));
	printf("Show cpu ultilization.\n");
	printf("%ld\%\n",syscall(341,a,b));
	return 0;
}
