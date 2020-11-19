#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * recv_msg(void * arg);
void * send_msg(void * arg);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t snd_thread, rcv_thread;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	pthread_create(&snd_thread, NULL, send_msg, NULL);
	pthread_detach(snd_thread);

	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		write(clnt_sock, &clnt_cnt, sizeof(clnt_cnt));
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&rcv_thread, NULL, recv_msg, (void*)&clnt_cnt);
		pthread_detach(rcv_thread);
		printf("\rConnected client %d\nserver : ", clnt_cnt);
		fflush(stdout);
	}
	close(serv_sock);
	return 0;
}
	
void * recv_msg(void * arg)
{
	int clnt_num=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	
	while ((str_len = read(clnt_socks[clnt_num-1], msg, sizeof(msg))) != 0)
	{
		msg[str_len] = 0;
		printf("\rclient %d : %sserver : ", clnt_num, msg);
		fflush(stdout);
		if (!strcmp(msg, "bye\n")) break;
	}
	printf("\rclient %d socket exit\nserver : ", clnt_num);
	fflush(stdout);
	pthread_mutex_lock(&mutx);
	close(clnt_socks[clnt_num-1]);
	pthread_mutex_unlock(&mutx);

	return NULL;
}

void * send_msg(void* arg) {
	char* all = "all";
	while (1) {
		char tmp[100];
		char char_c_cnt[10];
		printf("server : ");
		fgets(tmp, BUF_SIZE, stdin);
		char* ptr = strtok(tmp, " ");
		char* msg = strtok(NULL, "");

		if (!strcmp(ptr,all)) {
			for (int i = 1; i <= clnt_cnt; i++) {
				write(clnt_socks[i - 1], msg, strlen(msg));
				if (!strcmp(msg, "bye\n")) {
					//printf("\rclient %d socket exit\nserver : ", i);
					fflush(stdout);
					pthread_mutex_lock(&mutx);
					close(clnt_socks[i - 1]);
					pthread_mutex_unlock(&mutx);
				}
			}
		}
		else {
			for (int i = 1; i <= clnt_cnt; i++) {
				sprintf(char_c_cnt, "%d", i);
				if (!strcmp(ptr, char_c_cnt)) {
					write(clnt_socks[i - 1], msg, strlen(msg));
					if (!strcmp(msg, "bye\n")) {
						//printf("\rclient %d socket exit\nserver : ", i);
						fflush(stdout);
						pthread_mutex_lock(&mutx);
						close(clnt_socks[i - 1]);
						pthread_mutex_unlock(&mutx);
					}
					break;
				}
			}
		}
	}
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}