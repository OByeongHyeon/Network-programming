#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void error_handling(char *message);

struct packet {
	char data[4096];
	int data_size;
	int data_seq;
};

int main(int argc, char* argv[])
{
	int server_sock;
	struct sockaddr_in my_adr, your_adr;
	struct packet p;
	socklen_t adr_sz;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	server_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_sock == -1)
		error_handling("socket() error");

	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family = AF_INET;
	my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_adr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*) & my_adr, sizeof(my_adr)) == -1)
		error_handling("bind() error");
	
	int fd;

	memset((void*)&p, 0x00, sizeof(p));
	adr_sz = sizeof(your_adr);
	int start = 0;

	while (1) {
		recvfrom(server_sock, (char *)&p, sizeof(p), 0, (struct sockaddr*) & your_adr, &adr_sz);
		if (start == 0) {
			fd = open("my4.txt", O_CREAT | O_WRONLY | O_APPEND);	// 클라이언트가 sendto 한 패킷을 받으면 파일을 오픈
			if (fd == -1)
				error_handling("open() error!");
			start++;
		}
		
		if (!strcmp(p.data, "0x1A"))	// 루프를 빠져나가는 조건 즉, 종료조건
		{
			break;
		}

		write(fd, p.data, p.data_size);		// 파일에 업로드

		printf("%d byte data (seq %d) recieved.\n", p.data_size, p.data_seq);
	}
	close(fd);
	close(server_sock);
	printf("file close\n");
	printf("server close\n");
	return 0;
}



void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
