#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>
#include <openssl/aes.h>


#define DES_ENCRYPT 1
#define DES_DECRYPT 0
#define BLOCK_SIZE 128
#define KEY_SIZE 8

//typedef unsigned char DES_cblock[8];
//typedef /* const */ unsigned char const_DES_cblock[8];
//typedef unsigned int DES_key_schedule;

unsigned int des(unsigned char * msg, unsigned char * key, unsigned int msg_len, int mode)
{
    DES_key_schedule des_ks;
    DES_cblock des_key = {0,};
    DES_cblock iv = {0,};
    unsigned int i, result, padding;

    unsigned char block_in[BLOCK_SIZE] = {0,};
    unsigned char block_out[BLOCK_SIZE] = {0,};

    DES_string_to_key(key, &des_key);
    DES_set_key_checked(&des_key, &des_ks);

    memcpy(block_in, msg, msg_len);
    if (mode == 1)
    {
        if(msg_len < BLOCK_SIZE)
        {
            padding = BLOCK_SIZE - msg_len;
            block_in[BLOCK_SIZE-1]=padding;
        }
        DES_ncbc_encrypt(block_in, block_out, BLOCK_SIZE, &des_ks, &iv, DES_ENCRYPT);
		result = BLOCK_SIZE;
	}
    else if(mode == 2)
    {
        DES_ncbc_encrypt(block_in, block_out, BLOCK_SIZE, &des_ks, &iv, DES_DECRYPT);
        padding = block_out[BLOCK_SIZE-1];
        result = BLOCK_SIZE - padding;
    }
    memcpy(msg, block_out, BLOCK_SIZE);
	return result;
}
unsigned int aes_cbc(unsigned char * msg, unsigned char * key, unsigned int msg_len, int keylength, int mode)
{
	AES_KEY aes_key ={0,};
	unsigned char char_key[keylength/8];
	unsigned char iv[AES_BLOCK_SIZE]={0,};
    unsigned int i, result, padding;

    unsigned char block_in[1024] = {0,};
    unsigned char block_out[1024] = {0,};

	memset(iv,0,AES_BLOCK_SIZE);
	memset(char_key, 0,keylength/8);
	printf("key size %ld\n", sizeof(key));
	memcpy(char_key, key, sizeof(key));
    memcpy(block_in, msg, msg_len);
	int encrypt_size;
    if (mode == 1)
    {
		AES_set_encrypt_key(char_key, keylength, &aes_key);
        if(msg_len < AES_BLOCK_SIZE)
        {
            padding = AES_BLOCK_SIZE - msg_len;
            block_in[AES_BLOCK_SIZE-1]=padding;
        }
        AES_cbc_encrypt(block_in, block_out, msg_len, &aes_key, iv, AES_ENCRYPT);
		encrypt_size = ((msg_len+AES_BLOCK_SIZE ) / (keylength/8))*(keylength/8);
		printf("encrypt size %d \n", encrypt_size);
		result = encrypt_size;
	}
    else if(mode == 2)
    {
		AES_set_decrypt_key(char_key, keylength, &aes_key);
        AES_cbc_encrypt(block_in, block_out, msg_len, &aes_key, iv, AES_DECRYPT);
		padding = block_out[AES_BLOCK_SIZE-1];
        result = AES_BLOCK_SIZE - padding;
    }
    memcpy(msg, block_out, encrypt_size);
	printf("padding %d\n",result);
	return result;
}
unsigned int aes_ctr(unsigned char * msg, unsigned char * key, unsigned int msg_len, int mode)
{

}

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
int main()
{
    char inputFileName[256];
	char outputFileName[256];
	FILE *input_FD;
	FILE *output_FD;

	int mode;
	char key[8];
    int type;

	printf("encrypt[1] decrypt[2] : ");
	scanf("%d", &mode);

	printf("file : ");
	scanf("%s", inputFileName);

	printf("key : ");
	scanf("%s", key);

    printf("aes-cbc[1] aes-ctr[2] des[3] : ");
	scanf("%d", &type);

	if(mode == 1)
	{
		sprintf(outputFileName, "plain.enc");
	}
	else if (mode == 2)
	{
		sprintf(outputFileName, "plain.enc.dec");
	}
	else
	{
		printf("[!] Mode Error!\n");
		exit(1);
	}
	input_FD = fopen(inputFileName, "rb");
	output_FD = fopen(outputFileName, "wb");

	int fileSize = GetFileSize(input_FD);
	char buff[BLOCK_SIZE];
	int i;
	int len;
	int key_modified;

	memset(buff, 0, sizeof(buff));

    if(type == 1)
	{
		// aes-cbc
		int result;
		printf("aes-cbc\n");
		fread(buff, sizeof(char), fileSize,input_FD);
		result = aes_cbc(buff, key, fileSize, 256, mode);
		fwrite(buff, sizeof(char), result,output_FD);
		
		
	}
	else if (type == 2)
	{
		printf("aes-ctr\n");
		// aes-ctr
	}
	else if (type == 3)
	{
		printf("des\n");
		int result;
		if(fileSize<=BLOCK_SIZE)
		{
			fread(buff, sizeof(char), fileSize,input_FD);
			result = des(buff, key, fileSize, mode);
			fwrite(buff, sizeof(char), result,output_FD);
		}
		else
		{
			int loop = (fileSize-1)/BLOCK_SIZE;
			for (int i = 0; i<loop;i++)
			{
				fread(buff, sizeof(char), BLOCK_SIZE,input_FD);
				des(buff, key, BLOCK_SIZE, mode);
				fwrite(buff, sizeof(char), BLOCK_SIZE,output_FD);
			}
			int leftSize = fileSize%BLOCK_SIZE;

			if (mode==1)
			{
				fread(buff, sizeof(char), leftSize,input_FD);
				result = des(buff, key, leftSize, mode);
				
			}else
			{
				fread(buff, sizeof(char), BLOCK_SIZE,input_FD);
				result = des(buff, key, BLOCK_SIZE, mode);
			}		
			printf("fwrite %d\n", result);
			fwrite(buff, sizeof(char), result,output_FD);
		}
	}
	fclose(output_FD);
	fclose(input_FD);
}