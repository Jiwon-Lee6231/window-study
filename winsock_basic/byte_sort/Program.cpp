#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#include <stdio.h>

int main() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	unsigned int idata = 0x12345678;
	unsigned short sdata = 0x1234;
	printf("host:%#x network:%#x \n", idata, htonl(idata)); // ntohl
	printf("host:%#x network:%#x \n", sdata, htons(sdata)); // ntohs

	WSACleanup();

	return 0;
}