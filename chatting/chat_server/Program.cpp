#include "common.h"
#define PORT_NUM	10200
#define BLOG_SIZE	5
#define MAX_MSG_LEN 256

SOCKET SetTCPServer(short pnum, int blog);	// ��� ����
void EventLoop(SOCKET sock);

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);
	if (sock == -1) {
		perror("��� ���� ����");
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
	// ���� �ּ�, ��Ʈ��ũ �������̽� ����
	result = bind(sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (result == -1)
		return -1;

	result = listen(sock, blog);	// ��α� ť ����
	if (result == -1)
		return -1;

	printf("%s : %d Setup\n", inet_ntoa(server_addr.sin_addr), pnum);

	return sock;
}

SOCKET sock_base[FD_SETSIZE];	// ���� ����
HANDLE hev_base[FD_SETSIZE];	// �̺�Ʈ �ڵ� ����
int cnt;

// �̺�Ʈ �����ϰ� �迭 ����
HANDLE AddNetworkEvent(SOCKET sock, long net_event) {
	HANDLE hev = WSACreateEvent();	// ��Ʈ��ũ �̺�Ʈ

	sock_base[cnt] = sock;
	hev_base[cnt] = hev;
	cnt++;

	// ����, �̺�Ʈ ����.
	// ���Ͽ� ������ �̺�Ʈ�� �߻��ϸ� ��ȣ ���·� �ٲ�.
	WSAEventSelect(sock, hev, net_event);

	return hev;
}

void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);

void EventLoop(SOCKET sock) {
	// listen ���Ͽ��� Ŭ���̾�Ʈ ���� ��û
	// FD_ACCEPT ���� ��û ��Ʈ��ũ �̺�Ʈ ���
	AddNetworkEvent(sock, FD_ACCEPT);

	while (true) {
		// ��ȣ���°� �߻��� ������ ���
		int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);
		WSANETWORKEVENTS net_events;
		// � ��ȣ�� �ٲ������ Ȯ��
		WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);

		switch (net_events.lNetworkEvents) {
			case FD_ACCEPT: AcceptProc(index); break;
			case FD_READ: ReadProc(index); break;
			case FD_CLOSE: CloseProc(index); break;
		}
	}

	closesocket(sock);
}

// ���� ��û ó��
void AcceptProc(int index) {
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	SOCKET do_sock = accept(sock_base[0], (SOCKADDR*)&client_addr, &len);

	if (cnt == FD_SETSIZE) {
		printf("ä�� ���� �� ���� %s:%d �������� ���Ͽ���!\n",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));

		closesocket(do_sock);
		return;
	}

	// ���� ��û ������ ��ȯ�� �ۼ��� ���Ͽ�
	// �޽��� ����/���� �ݱ� ��û�� �� ��� ��ȣ ���·� �ٲ� �̺�Ʈ �߰�
	AddNetworkEvent(do_sock, FD_READ|FD_CLOSE);

	char msg[MAX_MSG_LEN];
	sprintf(msg, "%s:%d ����\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	// ä�ù濡 �ִ� ��� Ŭ���̾�Ʈ���� �޽��� ����
	printf("%s\n", msg);
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], msg, MAX_MSG_LEN, 0);
	}
}

// �޽��� ���� ó��
void ReadProc(int index) {
	char msg[MAX_MSG_LEN];
	recv(sock_base[index], msg, MAX_MSG_LEN, 0);

	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	getpeername(sock_base[index], (SOCKADDR*)&client_addr, &len);

	char smsg[MAX_MSG_LEN];
	sprintf(smsg, "[%s:%d]:%s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), msg);

	// ä�ù濡 �ִ� ��� Ŭ���̾�Ʈ���� �޽��� ����
	printf("%s\n", smsg);
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], smsg, MAX_MSG_LEN, 0);
	}
}

// ���� ����
void CloseProc(int index) {
	SOCKADDR_IN client_addr = { 0 };
	int len = sizeof(client_addr);
	getpeername(sock_base[index], (SOCKADDR*)&client_addr, &len);
	printf("[%s:%d]�� ���� \n",
		inet_ntoa(client_addr.sin_addr),
		ntohs(client_addr.sin_port));

	closesocket(sock_base[index]);
	WSACloseEvent(hev_base[index]);	// ��Ʈ��ũ �̺�Ʈ ��ü �ݱ�

	cnt--;
	sock_base[index] = sock_base[cnt];
	hev_base[index] = hev_base[cnt];

	char msg[MAX_MSG_LEN];
	sprintf(msg, "[%s:%d]�� ���� \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	for (int i = 1; i < cnt; i++) {
		send(sock_base[i], msg, MAX_MSG_LEN, 0);
	}
}