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
	int client_socket; // 클라이언트 핸들러를 저장할 변수

	struct sockaddr_in server_addr; // 서버 소켓 정보 저장할 구조체

	char buff[BUFF_SIZE+5]; // 메시지를 저장할 변수
	int str_len;	// 메시지의 길이를 저장할 변수

	client_socket = socket(PF_INET,SOCK_STREAM,0); // 클라이언트 소켓 생성
	if(-1==client_socket)
	{
		printf("client socket 생성 실패\n");
		exit(1);
	}

	memset(&server_addr,0,sizeof(server_addr)); // 서버소켓의 정보 저장
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(-1==connect(client_socket, (struct sockaddr*)&server_addr,sizeof(server_addr))) // 서버의 정보를 이용하여 connect 요청
	{
		printf("서버에 connect 실패\n");
		exit(1);

	}else printf("서버에 connect 성공\n");	
	
	while(1)
	{
	fputs ("전송할 메시지를 입력하시오.(종료 : bye) : ",stdout);
	fgets (buff, BUFF_SIZE, stdin); // 입력한 데이터를 buff에 저장

	printf("[client_snd] %s",buff);
	write(client_socket, buff, strlen(buff)+1); // 서버에 buff의 내용을 전송
	
	if(!strcmp(buff,"bye\n")) // 소켓 종료 조건
	{
		str_len = read (client_socket, buff, BUFF_SIZE); // 서버로부터 받은 메시지를 buff에 저장
		buff[str_len]=0;
		printf("[client_rcv] %s",buff); // 서버로부터 받은 메시지 출력
		close(client_socket);
		printf("client_socket 종료\n");
	
	return 0;
	}else{
		str_len = read (client_socket, buff, BUFF_SIZE);
		buff[str_len]=0;
		printf("[client_rcv] %s",buff);
	}
	}	
}
