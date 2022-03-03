#include "common.h"
#define PORT_NUM      10200
#define MAX_MSG_LEN   256

void RecvThreadPoint(void* param);

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);	//扩加 檬扁拳

	char server_ip[40] = "";
	printf("辑滚 IP : ");
	gets_s(server_ip, sizeof(server_ip));

	SOCKET sock;
	// 家南 积己
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
		return -1;

	SOCKADDR_IN server_addr = { 0 };	// 家南 林家
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(PORT_NUM);

	int result = 0;
	result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (result == -1)
		return -1;

	_beginthread(RecvThreadPoint, 0, (void*)sock);
	char msg[MAX_MSG_LEN] = "";
	while (true) {
		gets_s(msg, MAX_MSG_LEN);
		send(sock, msg, sizeof(msg), 0);	// 价脚
		if (strcmp(msg, "exit") == 0)
			break;
	}

	closesocket(sock);  
	WSACleanup();

	return 0;
}

void RecvThreadPoint(void* param) {
	SOCKET sock = (SOCKET)param;
	char msg[MAX_MSG_LEN];

	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);

	while (recv(sock, msg, MAX_MSG_LEN, 0) > 0) {
		printf("%s\n", msg);
	}

	closesocket(sock);
}