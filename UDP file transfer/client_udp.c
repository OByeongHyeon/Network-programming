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

int main(int argc, char *argv[])
{
	int client_sock;
	struct packet p;
	struct sockaddr_in your_adr;
	socklen_t adr_sz;

	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	client_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(client_sock==-1)
		error_handling("socket() error");
	
	memset(&your_adr, 0, sizeof(your_adr));
	your_adr.sin_family=AF_INET;
	your_adr.sin_addr.s_addr=inet_addr(argv[1]);
	your_adr.sin_port=htons(atoi(argv[2]));

	int fd;
	fd = open("my1.txt", O_RDONLY);
	if (fd == -1)
		error_handling("open() error!");
	
	int i = 1;
	memset((void*)&p, 0x00, sizeof(p));
	adr_sz = sizeof(your_adr);

	while (1) {
		p.data_size = read(fd, p.data, sizeof(p.data)); // 파일을 4096바이트씩 읽어서 packet에 저장한다.
		p.data_seq = i++;

		if (p.data_size == -1)
			error_handling("read() error!");
		
		sendto(client_sock, (char *)&p, sizeof(p), 0, (struct sockaddr*) & your_adr, adr_sz);

		printf("%d byte data (seq %d) send.\n", p.data_size, p.data_seq);

		if (p.data_size != 4096)	// 파일을 다 보낸 경우 0x1A 를 전송
		{
			strcpy(p.data, "0x1A");
			p.data_size = sizeof("0x1A");
			sendto(client_sock, (char*)&p, sizeof(p), 0, (struct sockaddr*) & your_adr, sizeof(your_adr));
			break;
		}
	}
	close(fd);
	close(client_sock);
	printf("file close\n");
	printf("client close\n");
	return 0;
}



void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}