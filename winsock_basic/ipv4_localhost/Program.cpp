#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#include <stdio.h>
#pragma warning(disable:4996)

IN_ADDR GetDefaultMyIP();

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	IN_ADDR addr;
	addr = GetDefaultMyIP();	// default ipv4 �ּ� ������
	printf("%s\n", inet_ntoa(addr));	// ipv4 �ּҸ� ���ڿ��� ���

	WSACleanup();

	return 0;
}

IN_ADDR GetDefaultMyIP() {
	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };
	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR) {		// ȣ��Ʈ �̸� ������
		return addr;
	}

	HOSTENT* ptr = gethostbyname(localhostname);	// ȣ��Ʈ ��Ʈ�� ������
	while (ptr && ptr->h_name) {
		if (ptr->h_addrtype == PF_INET) {		// ipv4 �ּ� Ÿ���� ��
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);		// �޸� ����
			break;
		}

		ptr++;
	}

	return addr; 
}

/*
* ���� ����ϴ� ���� �Լ�
* 
*  - ���� ���� �Լ�
*   SOCKET socket(int af, int type, int protocol);
* 
*  - ���� �ݴ� �Լ�
*   int closesocket(SOCKET sock);
* 
*  - ���ϰ� ���� ��Ʈ��ũ �������̽��� �����ϴ� �Լ�
*   int bind(SOCKET sock, const struct sockaddr *addr, int addrlen);
* 
*  - TCP ���������� ����ϴ� ������ ���� ���� �� �α� ť�� ũ�� �����ϴ� �Լ�
*   int listen(sock, int backlog);
* 
*  - ���� ��û �Լ�
*   int connect(SOCKET sock, const struct sockaddr otheraddr, int addrlen);
*  
*  - Ŭ���̾�Ʈ ���� ���� �Լ�
*   SOCKET accept(SOCKET sock, struct sockaddr *clientaddr, int *addrlen);
* 
*  - �޽��� ���� �Լ�
*   int send(SOCKET sock, const char * buf, int len, int flags);
* 
*  - �޽��� ���� �Լ�
*   int recv(SOCKET sock, char * buf, int len, int flags);
* 
*  - �񿬰� ������ �޽��� ���� �Լ�
*   int sendto(SOCKET sock, const char * buf, int len, int flags, const struct sockaddr * to, int addrlen);
* 
*  - �񿬰� ������ �޽��� ���� �Լ�
*   int recvfrom(SOCKET sock, char * buf, int len, int flags, struct sockaddr * from, int *addrlen);
* 
*/