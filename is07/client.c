#include "chat.h"

#define BLOCK_SIZE 256
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
	printf("key : ");
	scanf("%s", key);

	pthread_create(&send_thread, NULL, send_msg, (void *)serv_sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void *)serv_sock);

	pthread_join(send_thread, &thread_result);
	pthread_join(recv_thread, &thread_result);

	close(serv_sock);

	return 0;
}
