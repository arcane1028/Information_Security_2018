#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>
#include <openssl/aes.h>

#define BLOCK_SIZE 128

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
unsigned int aes_cbc(unsigned char * msg, unsigned char * key, unsigned int msg_len, int mode)
{
    AES_KEY aes_key={0,};
	unsigned char iv[AES_BLOCK_SIZE]={0,};
    unsigned char block_in[BLOCK_SIZE] = {0,};
    unsigned char block_out[BLOCK_SIZE] = {0,};
    unsigned int i, result, padding;

    memcpy(block_in, msg, msg_len);
    if (mode == 1)
    {
		AES_set_encrypt_key(key, 128, &aes_key);

        if(msg_len < BLOCK_SIZE)
        {
            padding = BLOCK_SIZE - msg_len;
            block_in[BLOCK_SIZE-1]=padding;
        }
        AES_cbc_encrypt(block_in, block_out, BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);
        result = BLOCK_SIZE;
	}
    else if(mode == 2)
    {
		AES_set_decrypt_key(key,128 , &aes_key);
        AES_cbc_encrypt(block_in, block_out, BLOCK_SIZE, &aes_key, iv, AES_DECRYPT);
		padding = block_out[BLOCK_SIZE-1];
        result = BLOCK_SIZE - padding;
    }
    memcpy(msg, block_out, BLOCK_SIZE);
	return result;
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
	char key[16]={0,};
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
	int i=0;
	int len;
	int key_modified;

	memset(buff, 0, sizeof(buff));
    int t=0;
    int loop = (fileSize-1)/BLOCK_SIZE;
    while ( 0 < (t = fread(buff, sizeof(char), BLOCK_SIZE, input_FD))){
        int res =0;
        if(type ==1){
            res = aes_cbc(buff, key, t, mode);
        }
        if (type ==3){
            res = des(buff, key, t, mode);
            
        }
        printf("\n res = %d\n", res);
        if(i<loop){
            fwrite(buff, sizeof(char), BLOCK_SIZE,output_FD);
        }else{
            fwrite(buff, sizeof(char), res,output_FD);
        }
        memset(buff, 0, sizeof(char)* BLOCK_SIZE);
        i++;
    }

	fclose(output_FD);
	fclose(input_FD);
}