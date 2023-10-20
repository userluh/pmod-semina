

#define BUFF_SIZE 1024 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

int server_socket;              //서버 디스크립터
int client_socket;              //클라이언트 디스크립터
int option;
struct sockaddr_in server_addr; //소켓에 주소와 포트를 할당하기 위해 struct 구조체 이용
char buff_receive[BUFF_SIZE+5];
char buff_send[BUFF_SIZE+5];
	
void *wri()
{
	while(1)
	{
		if(getchar() == '\n')
		{
			printf("server : ");
			fgets(buff_send, sizeof(buff_send), stdin);		
			write(client_socket, buff_send, strlen(buff_send)+1);
		}
	}
}

int main(void)
{
	pthread_t p_thread[1];      // 쓰레드 식별자
    int thread_id;
    int status;
	
	thread_id = pthread_create(&p_thread[0], NULL, wri, NULL);
    if(thread_id < 0)
	{
		perror("thread create error : ");
		exit(0);
	}
	
	memset(&server_addr, 0, sizeof(server_addr)); //메모리의 크기를 변경할 포인터, 초기화 값, 초기화 길이
	server_addr.sin_family = AF_INET;   //IPv4 인터넷 프로토콜
	server_addr.sin_port = htons(4000); //사용할 port 번호 4000
	server_addr.sin_addr.s_addr= inet_addr("192.168.0.108"); //자신의 서버주소 //32bit IPv4주소
	//htonl( INADDR_ANY) 는 주소를 지정해 주는 것 inet_addr("내 시스템의 IP ")로도 지정할 수 있습니다. 
	//그러나 프로그램이 실행되는 시스템 마다 IP 가 다를 것이므로 주소 지정을 고정 IP로 하지 않고 htonl(INADDR_ANY) 를 사용하는 것이 편리합니다.
		 
	//소켓 생성
	server_socket = socket(PF_INET, SOCK_STREAM, 0);    //TCP/IP에서는 SOCK_STREAM 을 UDP/IP에서는 SOCK_DGRAM을 사용
	if(-1 == server_socket) 
	{
		printf("socket() error\n"); 
		exit(1); //오류 확인
	}
	
	//소켓의 바인드 오류를 해결해주기위한 함수
	option=1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));

	//bind()함수를 이용하여 소켓에 server socket에 필요한 정보를 할당하고 커널에 등록
	if(-1 == bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
	{
		printf("bind error\n");
		exit(1);
	}
	
	//클라이언트 접속 요청 확인
	if( -1 == listen(server_socket,1)) 
	{
		printf("listen error\n");
		exit(1);
	}
	
	//client_addr_size = sizeof(client_addr); //client 주소 크기 대입
	client_socket = accept(server_socket, NULL, NULL);
	//accept()함수는 클라이언트의 접속 요청을 받아드리고 클라이언트와 통신하는 전용 소켓을 생성합니다
	//accept()로 접속요청을 허락시, 클라이언트와 통신을 하기 위해 커널이 자동으로 소켓 생성= client_socket
	if(-1 == client_socket) 
	{
		printf("클라이언트 연결 수락 실패\n");
		exit(1);
	}
		
	while(1)
	{
		read(client_socket, buff_receive, BUFF_SIZE);
		printf("client : %s", buff_receive);
	}
	
	return 0;
}
