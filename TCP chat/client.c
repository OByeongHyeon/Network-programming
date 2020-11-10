#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv)
{
	int client_socket; // client_socket 핸들러를 저장할 변수

	struct sockaddr_in server_addr; // 연결할 서버 소켓 정보 저장할 구조체

	char buff[BUFF_SIZE+5]; // 메시지를 저장할 변수	
	char *server_ip;
	char *client_ip = "192.168.0.7";	
	if(argc!=3){
		printf("Usage:%s <server ip> <port>\n",argv[0]);
		exit(0);
	}

	client_socket = socket(PF_INET,SOCK_STREAM,0); // 클라이언트 소켓 생성
	if(-1==client_socket)
	{
		printf("client socket 생성 실패\n");
		exit(1);
	}
	
	memset(&server_addr,0,sizeof(server_addr)); // 서버소켓의 정보 저장
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr=inet_addr(argv[1]);
	server_ip = inet_ntoa(server_addr.sin_addr);		

	if(-1==connect(client_socket, (struct sockaddr*)&server_addr,sizeof(server_addr))) // 서버의 정보를 이용하여 connect 요청
	{
		printf("서버에 connect 실패\n");
		exit(1);

	}else {
	write(client_socket, client_ip, strlen(client_ip)+1);		// 서버와 연결이 된경우 서버로 클라이언트의ip를 전송해준다.
	printf("Success connect to server!! if you want to quit, send 'bye' to server\n");
	}
	while(1)
	{
	printf("[%s->send]",client_ip);
	fputs (" ",stdout);
	fgets (buff, BUFF_SIZE, stdin); // 입력한 데이터를 buff에 저장
	
	write(client_socket, buff, strlen(buff)+1); // 서버에 buff의 내용을 전송
	
	if(!strcmp(buff,"bye\n")) // 소켓 종료 조건
	{
		
		printf("=====1:1 chat has ended!=====\nclient_socket end\n");
		close(client_socket);
	
		return 0;
	}else{
	 	read (client_socket, buff, BUFF_SIZE);
		printf("[%s->recv] %s",server_ip,buff);
	}
	}	
}
