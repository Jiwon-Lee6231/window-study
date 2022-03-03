#include "common.h"
#define PORT_NUM 10200
#define MAX_MSG_LEN 256

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);	// ���� �ʱ�ȭ

	char server_ip[40] = "";
	printf("���� IP : ");
	gets_s(server_ip, sizeof(server_ip));

	SOCKET sock;
	// ���� ����
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
		return -1;

	SOCKADDR_IN server_addr = { 0 };	// ���� �ּ�
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(PORT_NUM);

	// ���� �õ�
	int result = 0;
	result = connect(sock, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (result == -1)
		return -1;

	// Doit
	char msg[MAX_MSG_LEN] = "";
	while (true) {
		gets_s(msg, MAX_MSG_LEN);
		send(sock, msg, sizeof(msg), 0);
		if (strcmp(msg, "exit") == 0)
			break;

		recv(sock, msg, sizeof(msg), 0);
		printf("���� : %s\n", msg);
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}