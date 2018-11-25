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

	//printf("File Size = %d\n", fsize);
	return fsize;
}

int main(void){

    while(1){
        int command = 0;
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
                //Hash
                SHA256_CTX sha256;
                SHA256_Init(&sha256);
                SHA256_Update(&sha256, buff, len);
                SHA256_Final(hash, &sha256);

                char hex[strlen(hash)*2];

                for (int i = 0, j = 0; i < strlen(hash); ++i, j += 2)
                    sprintf(hex + j, "%02x", hash[i] & 0xff);

                printf("%s\n", hex);
                fwrite(hex, sizeof(char), 64,output_FD);
                fclose(input_FD);
                fclose(output_FD);
                break;
            }
            case 2:
            {
                char originFileName[] = "hash-sha256.txt";
                char inputFileName[64] = {0};
                FILE *origin_FD;
                FILE *input_FD;

                printf("Input File Name: ");
                scanf("%s", inputFileName);

                origin_FD = fopen(originFileName, "rb");
                input_FD = fopen(inputFileName, "rb");

                int originFileSize = GetFileSize(origin_FD);
                int inputFileSize = GetFileSize(input_FD);
                char originBuff[originFileSize+1];
                char buff[inputFileSize];
                char hash[32]={0,};
                memset(originBuff, 0, originFileSize+1);
                memset(buff, 0, inputFileSize);
                
                int len = fread(originBuff, sizeof(char), originFileSize, origin_FD);
                originBuff[originFileSize] ='\0';// 문자열 끝 
                printf("Original Hash : %s\n", originBuff);

                len = fread(buff, sizeof(char), inputFileSize, input_FD);
                //해쉬
                SHA256_CTX sha256;
                SHA256_Init(&sha256);
                SHA256_Update(&sha256, buff, len);
                SHA256_Final(hash, &sha256);

                char hex[strlen(hash)*2+1];

                for (int i = 0, j = 0; i < strlen(hash); ++i, j += 2)
                    sprintf(hex + j, "%02x", hash[i] & 0xff);
                hex[64] = '\0'; //문자열 끝
                printf("Generated Hash: %s\n", hex);

                if(strcmp(originBuff, hex)==0){
                    printf("EQUAL\n");
                }else{
                    printf("DIFFERENT\n");
                }

                fclose(input_FD);
                fclose(origin_FD);
                break;
            }
                break;
            case 3:
                printf("QUIT\n");
                exit(1);
            default:
                printf("command error\n");
        }
    }

}