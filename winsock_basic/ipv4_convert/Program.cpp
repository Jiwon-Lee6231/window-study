#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#include <stdio.h>
#pragma warning(disable:4996)

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	IN_ADDR addr;
	addr.s_addr = htonl(12 << 24 | 34 << 16 | 56 << 8 | 78); // 12.34.56.78
	printf("%s\n", inet_ntoa(addr));

	u_int naddr = inet_addr("192.168.34.0");
	u_int haddr = ntohl(naddr);
	printf("%d.%d.%d.%d\n", haddr >> 24, (u_char)(haddr >> 16),(u_char)(haddr >> 8),(u_char)haddr);

	WSACleanup();

	return 0;
}