#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

    // 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("소켓 생성 실패");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1880);  // 포트 번호 설정
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("바인딩 실패");
        exit(1);
    }

    // 클라이언트 연결 대기
    if (listen(server_socket, 5) == -1) {
        perror("대기 실패");
        exit(1);
    }

    printf("서버 대기 중...\n");

    // 클라이언트 연결 수락
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("연결 수락 실패");
        exit(1);
    }

    printf("클라이언트 연결됨\n");

    close(server_socket);

    // 데이터 수신 및 처리
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, BUFF_SIZE);
        

        printf("수신된 데이터: %s\n", buffer);

        // 원하는 동작 수행
      /*  if (strcmp(buffer, "r") == 0) {
            // "r" 문자를 수신했을 때 원하는 동작 수행
            printf("라즈베리 파이에서 'r'을 수신했습니다. 원하는 동작을 수행하세요.\n");
        }*/
    }

    // 연결 종료
    close(client_socket);
    

    return 0;
}