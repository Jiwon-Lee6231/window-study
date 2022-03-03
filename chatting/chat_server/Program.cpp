#include "common.h"
#define PORT_NUM	10200
#define BLOG_SIZE	5
#define MAX_MSG_LEN 256

SOCKET SetTCPServer(short pnum, int blog);	// 대기 소켓
void EventLoop(SOCKET sock);

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);
	if (sock == -1) {
		perror("대기 소켓 오류");
	}
	else {
		EventLoop(sock);
	}

	WSACleanup();

	return 0;
}

SOCKET SetTCPServer(short pnum, int blog) {
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
		return -1;

	SOCKADDR_IN server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = GetDefaultMyIP();
	server_addr.sin_port = htons(PORT_NUM);

	int result = 0;
	// 소켓 주소, 네트워크 인터페이스 결합
	result = bind(sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (result == -1)
		return -1;

	result = listen(sock, blog);	// 백로그 큐 설정
	if (result == -1)
		return -1;

	printf("%s : %d Setup\n", inet_ntoa(server_addr.sin_addr), pnum);

	return sock;
}

SOCKET sock_base[FD_SETSIZE];	// 소켓 보관
HANDLE hev_base[FD_SETSIZE];	// 이벤트 핸들 보관
int cnt;

// 이벤트 생성하고 배열 보관
HANDLE AddNetworkEvent(SOCKET sock, long net_event) {
	HANDLE hev = WSACreateEvent();	// 네트워크 이벤트

	sock_base[cnt] = sock;
	hev_base[cnt] = hev;
	cnt++;

	// 소켓, 이벤트 매핑.
	// 소켓에 설정한 이벤트가 발생하면 신호 상태로 바꿈.
	WSAEventSelect(sock, hev, net_event);

	return hev;
}

void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);

void EventLoop(SOCKET sock) {
	// listen 소켓에서 클라이언트 연결 요청
	// FD_ACCEPT 연결 요청 네트워크 이벤트 상수
	AddNetworkEvent(sock, FD_ACCEPT);

	while (true) {
		// 신호상태가 발생할 때까지 대기
		int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);
		WSANETWORKEVENTS net_events;
		// 어떤 신호로 바뀐것인지 확인
		WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);

		switch (net_events.lNetworkEvents) {
			case FD_ACCEPT: AcceptProc(index); break;
			case FD_READ: ReadProc(index); break;
			case FD_CLOSE: CloseProc(index); break;
		}
	}

	closesocket(sock);
}

// 연결 요청 처리
void AcceptProc(int index) {
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	SOCKET do_sock = accept(sock_base[0], (SOCKADDR*)&client_addr, &len);

	if (cnt == FD_SETSIZE) {
		printf("채팅 방이 꽉 차서 %s:%d 입장하지 못하였음!\n",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));

		closesocket(do_sock);
		return;
	}

	// 연결 요청 수락후 반환한 송수신 소켓에
	// 메시지 수신/연결 닫기 요청이 올 경우 신호 상태로 바꿀 이벤트 추가
	AddNetworkEvent(do_sock, FD_READ|FD_CLOSE);

	char msg[MAX_MSG_LEN];
	sprintf(msg, "%s:%d 입장\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	// 채팅방에 있는 모든 클라이언트에게 메시지 전송
	printf("%s\n", msg);
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], msg, MAX_MSG_LEN, 0);
	}
}

// 메시지 수신 처리
void ReadProc(int index) {
	char msg[MAX_MSG_LEN];
	recv(sock_base[index], msg, MAX_MSG_LEN, 0);

	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	getpeername(sock_base[index], (SOCKADDR*)&client_addr, &len);

	char smsg[MAX_MSG_LEN];
	sprintf(smsg, "[%s:%d]:%s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), msg);

	// 채팅방에 있는 모든 클라이언트에게 메시지 전송
	printf("%s\n", smsg);
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], smsg, MAX_MSG_LEN, 0);
	}
}

// 연결 종료
void CloseProc(int index) {
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	getpeername(sock_base[index], (SOCKADDR*)&client_addr, &len);
	printf("[%s:%d]님 나감 \n",
		inet_ntoa(client_addr.sin_addr),
		ntohs(client_addr.sin_port));

	closesocket(sock_base[index]);
	WSACloseEvent(hev_base[index]);	// 네트워크 이벤트 개체 닫기

	cnt--;
	sock_base[index] = sock_base[cnt];
	hev_base[index] = hev_base[cnt];

	char msg[MAX_MSG_LEN];
	sprintf(msg, "[%s:%d]님 나감 \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], msg, MAX_MSG_LEN, 0);
	}
}