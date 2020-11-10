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
	int server_socket = 0;		// server_socket 핸들러를 저장할 변수 
	int client_socket = 0;		// client_socket 핸들러를 저장할 변수
	int client_addr_size = 0;	// client 구조체 크기

	struct sockaddr_in server_addr;	// 서버 소켓 정보를 저장할 구조체 생성
	struct sockaddr_in client_addr; // 클라이언트 소켓 정보를 저장할 구조체 생성

	char buff_rcv[BUFF_SIZE];
	char buff_snd[BUFF_SIZE];
	char *server_ip;
	char client_ip[15];	// 클라이언트의 ip를 받을 공간
	memset(&server_addr, 0x00, sizeof(server_addr));	//server 정보 초기화 
	memset(&client_addr, 0x00, sizeof(client_addr));	//client 정보 초기화

	if(argc != 2) {
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	server_socket = socket(PF_INET, SOCK_STREAM, 0);	// server_socket 생성
	if(-1==server_socket)					// 예외 처리
	{
		printf("server socket 생성 실패\n");
		exit(1);
	}
	
	memset(&server_addr, 0, sizeof(server_addr));		// 서버 소켓 초기화
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.6");
	server_ip = inet_ntoa(server_addr.sin_addr);
	printf("서버 ip : %s, port = %d\n",server_ip,ntohs(server_addr.sin_port));

	if(-1==bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("bind() 실행 에러\n");
		exit(1);
	}
	

	if(-1==listen(server_socket,5))
	{
		printf("listen() 실행 실패\n");
		exit(1);
	}

	while(1){		
	client_addr_size = sizeof(client_addr); // 클라이언트 정보 구조체 크기
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size); // 연결한 클라이언트 소켓으로 부터 데이터를 받아들임
	
	if(-1==client_socket) // 예외 처리 -> 클라이언트 연결 실패하더라도 종료되지않고, 루프에 의해  다시 accept로 대기를 한다.
	{
		printf("클라이언트 연결 실패\n");
	}else{
		read(client_socket,client_ip, 15); 	// 연결이 된 경우 클라이언트의 ip를 받는다.	
		printf("Connected from %s...\n", client_ip);
		}
	
	while(1)
	{
		read(client_socket, buff_rcv, BUFF_SIZE);  // 받은 데이터를 읽는다.
		printf("[%s->recv] %s",client_ip,buff_rcv);
	
		if(!strcmp(buff_rcv,"bye\n")) // 연결 종료 조건
		{
			close(client_socket);
			printf("=====1:1 chat has ended=====\nclient_socket end\n\n");
			break;
		}
		printf("[%s->send]",server_ip);
		fputs (" ",stdout);
		fgets (buff_snd, BUFF_SIZE, stdin); // 입력한 데이터를 buff에 저장

		write(client_socket,buff_snd,strlen(buff_snd)+1); // 클라이언트에게 받은 데이터를 다시 보낸다.
	}
	}
}



