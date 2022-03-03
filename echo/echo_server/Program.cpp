#include "common.h"
#define PORT_NUM 10200
#define BLOG_SIZE 5
#define MAX_MSG_LEN 256

SOCKET SetTCPServer(short pnum, int blog);	// ��Ʈ ��ȣ, ��α� ũ��
void AcceptLoop(SOCKET sock);
// void Doit(SOCKET do_sock);
void Doit(void* param);

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);	// ���� �ʱ�ȭ
	
	// ��� ���� ����
	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);
	if (sock == -1) {
		perror("��� ���� ����");
		WSACleanup();
		return 0;
	}

	AcceptLoop(sock);

	WSACleanup();	// ���� ����

	return 0;
}

SOCKET SetTCPServer(short pnum, int blog) {
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		// ���� ���� (ipv4 = AP_INET, ipv6 = AP_INET6)
	if (sock == -1)
		return -1;

	SOCKADDR_IN server_addr = { 0 };	// ���� �ּ�
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = GetDefaultMyIP();
	server_addr.sin_port = htons(PORT_NUM);

	int result = 0;
	// ��Ʈ��ũ �������̽��� ���� ����
	result = bind(sock, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (result == -1)	// SOCKET_ERROR
		return -1;

	// ��α� ť ũ�� ����
	result = listen(sock, blog);
	if (result == -1)	// SOCKET_ERROR
		return -1;

	printf("%s : %d Setup\n", inet_ntoa(server_addr.sin_addr), pnum);

	return sock;
}

void AcceptLoop(SOCKET sock) {
	SOCKET do_sock;
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);

	while (true) {
		do_sock = accept(sock, (SOCKADDR *)&client_addr, &len);		// ���� ����

		if (do_sock == -1) {
			perror("accept fail");
			break;
		}

		printf("%s : %d�� ���� ��û ���� \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		// Doit(do_sock);
		_beginthread(Doit, 0, (void*)do_sock);
	}

	closesocket(sock);
}

void Doit(void* param) {
	SOCKET do_sock = (SOCKET)param;
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);

	// ��� ���� �ּ� �˾Ƴ���
	getpeername(do_sock, (SOCKADDR*)&client_addr, &len);
	char msg[MAX_MSG_LEN] = "";

	while (recv(do_sock, msg, sizeof(msg), 0) > 0) {
		// printf("recv:%s\n", msg);
		printf("%s:%d�κ��� recv:%s\n",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port),
			msg);
		send(do_sock, msg, sizeof(msg), 0);
	}
	
	printf("%s:%d�� ��� ����\n",
		inet_ntoa(client_addr.sin_addr),
		ntohs(client_addr.sin_port));

	closesocket(do_sock);
}