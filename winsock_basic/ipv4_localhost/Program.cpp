#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#include <stdio.h>
#pragma warning(disable:4996)

IN_ADDR GetDefaultMyIP();

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	IN_ADDR addr;
	addr = GetDefaultMyIP();	// default ipv4 주소 얻어오기
	printf("%s\n", inet_ntoa(addr));	// ipv4 주소를 문자열로 출력

	WSACleanup();

	return 0;
}

IN_ADDR GetDefaultMyIP() {
	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };
	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR) {		// 호스트 이름 얻어오기
		return addr;
	}

	HOSTENT* ptr = gethostbyname(localhostname);	// 호스트 엔트리 얻어오기
	while (ptr && ptr->h_name) {
		if (ptr->h_addrtype == PF_INET) {		// ipv4 주소 타입일 때
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);		// 메모리 복사
			break;
		}

		ptr++;
	}

	return addr; 
}

/*
* 자주 사용하는 윈속 함수
* 
*  - 소켓 생성 함수
*   SOCKET socket(int af, int type, int protocol);
* 
*  - 소켓 닫는 함수
*   int closesocket(SOCKET sock);
* 
*  - 소켓과 로컬 네트워크 인터페이스를 결합하는 함수
*   int bind(SOCKET sock, const struct sockaddr *addr, int addrlen);
* 
*  - TCP 프로토콜을 사용하는 소켓의 동시 연결 백 로그 큐의 크기 설정하는 함수
*   int listen(sock, int backlog);
* 
*  - 연결 요청 함수
*   int connect(SOCKET sock, const struct sockaddr otheraddr, int addrlen);
*  
*  - 클라이언트 연결 수락 함수
*   SOCKET accept(SOCKET sock, struct sockaddr *clientaddr, int *addrlen);
* 
*  - 메시지 전송 함수
*   int send(SOCKET sock, const char * buf, int len, int flags);
* 
*  - 메시지 수신 함수
*   int recv(SOCKET sock, char * buf, int len, int flags);
* 
*  - 비연결 상태의 메시지 전송 함수
*   int sendto(SOCKET sock, const char * buf, int len, int flags, const struct sockaddr * to, int addrlen);
* 
*  - 비연결 상태의 메시지 수신 함수
*   int recvfrom(SOCKET sock, char * buf, int len, int flags, struct sockaddr * from, int *addrlen);
* 
*/