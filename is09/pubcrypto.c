#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#define KEY_LENGTH  1024
#define PUB_EXP     3
#define BLOCK_SIZE  128
#define MSG_SIZE    117

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
    char inputFileName[]= "plain";
	char encryptFileName[] = "plain.enc";
    char decryptFileName[] = "plain.enc.dec";

	FILE *input_FD;
	FILE *encrypt_FD;
    FILE *decrypt_FD;

    int pri_len;
    int pub_len;
    char *pri_key;
    char *pub_key;
    // 키생성
    RSA *keypair = RSA_generate_key(KEY_LENGTH, PUB_EXP, NULL, NULL);

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());
    // PEM 포멧으로 출력
    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    pri_len = BIO_pending(pri);
    pub_len = BIO_pending(pub);

    pri_key = malloc(pri_len +1);
    pub_key = malloc(pub_len+1);
    // 문자열 출력
    BIO_read(pri, pri_key, pri_len);
    BIO_read(pub, pub_key, pub_len);

    pri_key[pri_len] = '\0';
    pub_key[pub_len] = '\0';
    // 파일 쓰기
    FILE *key_file;
    key_file = fopen("prkey.pem", "w");
    fprintf(key_file, "%s", pri_key);
    fclose(key_file);

    key_file = fopen("pukey.pem", "w");
    fprintf(key_file, "%s", pub_key);
    fclose(key_file);

    key_file = fopen("rsa.key", "w");
    RSA_print_fp(key_file, keypair, 0);
    fclose(key_file);

    free(pri_key);
    free(pub_key);

    input_FD = fopen(inputFileName, "rb");
	encrypt_FD = fopen(encryptFileName, "wb");
    // encrypt
    int fileSize = GetFileSize(input_FD);
	char buff[MSG_SIZE];
	int len;
    int t=0;

    memset(buff, 0, sizeof(buff));
    //파일 끝까지 실행
    while ( 0 < (t = fread(buff, sizeof(char), MSG_SIZE, input_FD))){
        int res = 0;
        unsigned char block_in[BLOCK_SIZE] = {0,};
        unsigned char block_out[BLOCK_SIZE] = {0,};
        memcpy(block_in, buff, t);
        // 공개키 암호화
        res = RSA_public_encrypt(t, block_in, block_out, keypair, RSA_PKCS1_PADDING);
        fwrite(block_out, sizeof(char), res,encrypt_FD);
        memset(buff, 0, sizeof(char)* MSG_SIZE); //초기화
    }
    fclose(encrypt_FD);
	fclose(input_FD);

    //decrypt
    char buff2[BLOCK_SIZE];
    encrypt_FD = fopen(encryptFileName, "rb");
    decrypt_FD = fopen(decryptFileName, "wb");
    fileSize = GetFileSize(encrypt_FD);
	memset(buff2, 0, sizeof(buff2));
    t=0;

    while ( 0 < (t = fread(buff2, sizeof(char), BLOCK_SIZE, encrypt_FD))){
        int res = 0;
        unsigned char block_in[BLOCK_SIZE] = {0,};
        unsigned char block_out[BLOCK_SIZE] = {0,};
        memcpy(block_in, buff2, t);
        //개인키 복호화
        res = RSA_private_decrypt(t, block_in, block_out, keypair, RSA_PKCS1_PADDING);
        fwrite(block_out, sizeof(char), res,decrypt_FD);
        memset(buff2, 0, sizeof(char)* BLOCK_SIZE);
    }

    fclose(decrypt_FD);
    fclose(encrypt_FD);
}