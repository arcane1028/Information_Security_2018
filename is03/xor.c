#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetFileSize(FILE *fp)
{
	int offset_bkup;
	int fsize;

	offset_bkup = ftell(fp);
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, offset_bkup, SEEK_SET);

	printf("File Size = %d\n", fsize);
	return fsize;
}

void enc(char* buff, int key)
{
	*buff = *buff^key;
}
void dec(char *buff, int key)
{
	*buff = *buff ^ key;
}

void main()
{
	char inputFileName[256];
	char outputFileName[256];
	FILE *input_FD;
	FILE *output_FD;

	int mode;
	char key[26];

	printf(">> Input file name : ");
	scanf("%s", inputFileName);

	printf("input mode [0 : ENC, 1 : DEC] :");
	scanf("%d", &mode);

	printf("input key : ");
	scanf("%s", key);

	if(mode == 0)
	{
		sprintf(outputFileName, "encrypt.txt");
	}
	else if (mode == 1)
	{
		sprintf(outputFileName, "decrypt.txt");
	}
	else
	{
		printf("[!] Mode Error!\n");
		exit(1);
	}
	input_FD = fopen(inputFileName, "rb");
	output_FD = fopen(outputFileName, "wb");

	int fileSize = GetFileSize(input_FD);
	char buff;
	int i;
	int len;
	int key_modified;

	for(i=0;i<fileSize;i++)
	{
		fread(&buff, sizeof(char), 1, input_FD);

		len = i%strlen(key);
		key_modified = key[len];
		if (mode==0)
		{
			enc(&buff, key_modified);
		}
		else if (mode == 1)
		{
			dec(&buff, key_modified);
		}

		fwrite(&buff, sizeof(char), 1, output_FD);

	}

	fclose(output_FD);
	fclose(input_FD);
}

