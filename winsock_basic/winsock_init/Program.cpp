#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
// MAKEWORD(2,2) == 0x0202
#include <stdio.h>

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// ±¸Çö
	printf("wHighVersion:%#x\n", wsadata.wHighVersion);
	printf("wVersion:%#x\n", wsadata.wVersion);
	printf("szDescription:%s\n", wsadata.szDescription);
	printf("szSystemStatus:%s\n", wsadata.szSystemStatus);
	printf("iMaxSocket:%d\n", wsadata.iMaxSockets);
	printf("iMaxUdpDg:%d\n", wsadata.iMaxUdpDg);

	WSACleanup();

	return 0;
}