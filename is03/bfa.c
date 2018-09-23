#include <stdio.h>
#include <stdlib.h>

int main()
{
	int answer = 6;
	printf("3 * 2 = ?\n");

	int input;
	while(1)
	{
		printf("\n>> Input : ");
		scanf("%d",&input);

		if(answer == input)
		{
			printf("[!] Great Job~!\n");
			exit(1);
		}
		else
		{
			printf("[?] %d is not answer\n", input);
		}
	}
}

