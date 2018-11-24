#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

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

int main(void){
    int command;
    printf("[1] Generate Hash [2] Hash Check [3] Quit\n");
    scanf("%d", &command);

    switch(command){
        case 1:
        {
            char inputFileName[] = "hash.txt";
            char outputFileName[] = "hash-sha256.txt";
            FILE *input_FD;
            FILE *output_FD;

            input_FD = fopen(inputFileName, "rb");
	        output_FD = fopen(outputFileName, "wb");

            int fileSize = GetFileSize(input_FD);
            char buff[fileSize];
            char hash[32]={0,};
            memset(buff, 0, fileSize);

            int len = fread(buff, sizeof(char), fileSize, input_FD);
            printf("read length %d\n", len);
            //buff[fileSize]='\0';
            printf("%s\n", buff);

            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, buff, strlen(buff));
            SHA256_Final(hash, &sha256);

            printf("hash length %d\n", strlen(hash));

            char hex[strlen(hash)*2];

            for (int i = 0, j = 0; i < strlen(hash); ++i, j += 2)
                sprintf(hex + j, "%02x", hash[i] & 0xff);

            printf("%s %d\n", hex, strlen(hex));
            fwrite(hex, sizeof(char), strlen(hex),output_FD);
            fclose(input_FD);
            fclose(output_FD);
            break;
        }
        case 2:

            break;
        case 3:
            exit(1);
        default:
            printf("command error\n");
    }

}