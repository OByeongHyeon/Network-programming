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
	int client_addr_size = 0;	// client 구조체 사이즈

	struct sockaddr_in server_addr;	// 서버 소켓 정보를 저장할 구조체 생성
	struct sockaddr_in client_addr; // 클라이언트 소켓 정보를 저장할 구조체 생성

	char buff_rcv[BUFF_SIZE];
	char buff_snd[BUFF_SIZE];

	memset(&server_addr, 0x00, sizeof(server_addr));	//서버정보 초기화 
	memset(&client_addr, 0x00, sizeof(client_addr));	//클라이언트정보 초기화

	server_socket = socket(PF_INET, SOCK_STREAM, 0);	// 서버 소켓생성
	if(-1==server_socket)					// 오류 처리
	{
		printf("server socket 생성 실패\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));		// 서버 소켓 초기화
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(-1==bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) // 초기화한 정보로 bind
	{
		printf("bind() 실행 에러\n");
		exit(1);
	}

	if(-1==listen(server_socket,5))	// 대기 클라이언트 최대 5개 가능
	{
		printf("listen() 실행 실패\n");
		exit(1);
	}
	while(1){		
	client_addr_size = sizeof(client_addr); // 클라이언트 정보 구조체 크기
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size); // 연결한 클라이언트 소켓으로 부터 데이터를 받아들임
	
	if(-1==client_socket) // 오류 처리 -> 클라이언트 연결 실패하더라도 종료되지않고, 루프에 의해  다시 accept로 대기를 한다.
	{
		printf("클라이언트 연결 실패\n");
	}else{

	while(1)
	{
		read(client_socket, buff_rcv, BUFF_SIZE);  // 받은 데이터를 읽는다.
		printf("[server_rcv] %s",buff_rcv);
		
		printf("[server_snd] %ld:%s",strlen(buff_rcv)-1,buff_rcv);
		sprintf(buff_snd,"%ld:%s",strlen(buff_rcv)-1,buff_rcv);
		write(client_socket,buff_snd,strlen(buff_snd)); // 클라이언트에게 받은 데이터를 다시 보낸다.
		
		if(!strcmp(buff_rcv,"bye\n")) // 연결 종료 조건
		{
			close(client_socket);
			break;
		}
	}
		}
	}
}

