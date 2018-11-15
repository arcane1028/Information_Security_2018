#include "chat.h"

#define BLOCK_SIZE 128
#define KEY_SIZE 8

TALK	msg;
char key[KEY_SIZE]={0,};

unsigned int des(unsigned char * buf, unsigned char * key, unsigned int msg_len, int mode)
{
    DES_key_schedule des_ks;
    DES_cblock des_key = {0,};
    DES_cblock iv = {0,};
    unsigned int i, result, padding;

    unsigned char block_in[BLOCK_SIZE] = {0,};
    unsigned char block_out[BLOCK_SIZE] = {0,};

    DES_string_to_key(key, &des_key);
    DES_set_key_checked(&des_key, &des_ks);

    memcpy(block_in, buf, msg_len);
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
    memcpy(buf, block_out, BLOCK_SIZE);
	return result;
}
void make_key(char * name){
	 int pri_len;
    int pub_len;
    char *pri_key;
    char *pub_key;
	char filename[64]={0,};
	//filename = malloc(64);
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
	memset(filename, 0, 64);
	memcpy(filename, name, 64);
	strcat(filename, "_prkey.pem");
    key_file = fopen(filename, "w");
    fprintf(key_file, "%s", pri_key);
    fclose(key_file);

	memset(filename, 0, 64);
	memcpy(filename, name, 64);
	strcat(filename, "_pukey.pem");
    key_file = fopen(filename, "w");
    fprintf(key_file, "%s", pub_key);
    fclose(key_file);

	memset(filename, 0, 64);
	memcpy(filename, name, 64);
	strcat(filename, "_rsa.key");
    key_file = fopen(filename, "w");
    RSA_print_fp(key_file, keypair, 0);

    fclose(key_file);
    free(pri_key);
    free(pub_key);
	//free(filename);
}

void * send_msg(void * arg)
{
	int sock = (int)arg;

	while(1) {
		fgets(msg.msg, MSG_SIZE, stdin);
		printf("\033[F\033[J");
		fprintf(stdout, "[%s] %s", msg.name, msg.msg);
		
		des(msg.msg, key, MSG_SIZE, 1);
		
		write(sock, (void *)&msg, sizeof(TALK));
        memset(msg.msg, 0x0, MSG_SIZE);
	}
}

void * recv_msg(void * arg)
{
	int sock = (int)arg;
	int str_len;

	while(1) {
		TALK tmp;
		str_len = read(sock, (void *)&tmp, sizeof(TALK));
		if ( -1 == str_len ) {
			return (void *)1;
		}
		des(tmp.msg, key, MSG_SIZE, 2);
		fprintf(stdout, "[%s] %s", tmp.name, tmp.msg);
	}
}

int main(int argc, char *argv[])
{
	int					serv_sock;
	struct sockaddr_in	serv_addr;
	pthread_t			send_thread, recv_thread;
	void *				thread_result;
	char *filepath;
	filepath = malloc(64);
	memset(filepath, 0x0, 64);

	// SIG HANDLER
	signal(SIGINT, (void *)handler);

	if ( argc != 4 ) {
		fprintf(stderr, "Usage : %s <ip> <port> <name>\n", argv[0]);
		exit(1);
	}
	sprintf(msg.name, "%s", argv[3]);

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == serv_sock ) {
		fprintf(stderr, "[!] ERROR : Socket()\n");
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr(argv[1]);
	serv_addr.sin_port			= htons(atoi(argv[2]));

	if ( -1 == connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ) {
		fprintf(stderr, "[!] ERROR : Connect()\n");
	}
	//키 생성
	memcpy(filepath, msg.name, 64);
	strcat(filepath, "_pukey.pem");
	printf("file name: %s\n", filepath);

	if (access(filepath, F_OK) == -1){
		make_key(msg.name);
		printf("complete key\n");
	}
	printf("test\n");
	if(strcmp("aaa", msg.name)==0){
		printf("key : ");
		scanf("%s", key);
	
		BIO *pub = BIO_new(BIO_s_file());
		if (BIO_read_filename(pub, "bbb_pukey.pem")<=0){
			printf("send error \n");
		}
		RSA *keypair = PEM_read_bio_RSAPublicKey(pub, NULL, NULL, NULL);
		printf("\n test 1 \n");
		unsigned char block_in[BLOCK_SIZE] = {0,};
        unsigned char block_out[BLOCK_SIZE] = {0,};
		memcpy(block_in, key, KEY_SIZE);
		printf("\n test 2 %s\n", block_in);
		RSA_public_encrypt(strlen(key), block_in, block_out, keypair, RSA_PKCS1_PADDING);
		printf("\n test 3 %s\n", block_out);
		write(serv_sock, (void *)block_out, sizeof(block_out));
        
	}else{
		printf("bbb\n");
		unsigned char block_in[BLOCK_SIZE] = {0,};
        unsigned char block_out[BLOCK_SIZE] = {0,};

		read(serv_sock, (void *)block_in, sizeof(block_in));
		
		BIO *pub = BIO_new(BIO_s_file());

		if (BIO_read_filename(pub, "bbb_prkey.pem")<=0){
			printf("read error\n");
		}
		RSA *keypair = PEM_read_bio_RSAPrivateKey(pub, NULL, NULL, NULL);
		printf("key test\n");

		RSA_private_decrypt(BLOCK_SIZE, block_in, block_out, keypair, RSA_PKCS1_PADDING);
		printf("key %s\n",block_out);

		memcpy(key, block_out, KEY_SIZE);
		printf("read key complete %s %d\n", key, sizeof(key));
	}
	
	pthread_create(&send_thread, NULL, send_msg, (void *)serv_sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void *)serv_sock);

	pthread_join(send_thread, &thread_result);
	pthread_join(recv_thread, &thread_result);

	close(serv_sock);
	free(filepath);

	return 0;
}
