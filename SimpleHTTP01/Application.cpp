#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")
const char* familyToString(int af_family)
{
	switch (af_family)
	{
	case AF_UNIX:
		return "unix";
	case AF_INET:
		return "inet4";
	case AF_INET6:
		return "inet6";
	default:
		return "";
	}
}
int main(int argc, char** argv)
{
	WSADATA wsaData = {};
	int err = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (err != 0)
	{
		printf("WSAStartup failed with error code %d\n", err);
		return 0;
	}
	printf("HTTP 01\n");

	const SOCKET sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sd == INVALID_SOCKET)
	{
		const int wsaGetLastError = WSAGetLastError();
		printf("failed to create socket %d\n", wsaGetLastError);
		WSACleanup();
		return 1;
	}
	addrinfo hints = {};
	
	
	addrinfo* result = nullptr;
	
	err = getaddrinfo("localhost", nullptr, &hints, &result);
	if (err != 0)
	{
		printf("failed to retrieve data from getaddrinfo %d\n", err);

	}
	char stringbuf[46];
	DWORD stringbuf_s = 46;
	for (addrinfo* child = result; child != nullptr; child = child->ai_next)
	{
		printf("Family %s\n", familyToString(child->ai_family));
		auto foo = (sockaddr_in*)child->ai_addr;
		printf("IP Address %s\n", inet_ntop(child->ai_family, &foo->sin_addr, stringbuf, stringbuf_s));
		printf("Length of this socketaddr %d\n", child->ai_addrlen);
	}
	freeaddrinfo(result);
	sockaddr_in to_host = {};
	to_host.sin_port = htons(8080);
	in_addr baz_buf;
	inet_pton(AF_INET, "127.0.0.1", &baz_buf);
	to_host.sin_addr = baz_buf;
	to_host.sin_family = AF_INET;
	

	err = connect(sd, reinterpret_cast<sockaddr*>(&to_host), sizeof(to_host));
	if(err != 0)
	{
		printf("Failed to connect");
		closesocket(sd);
		WSACleanup();
		return 2;
	}

	auto buf = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	send(sd, buf, strlen(buf), 0);
	char foo[2048] = { 0 };
	recv(sd, foo, 2048, 0);
	printf("RECV %s\n", foo);
	closesocket(sd);
	WSACleanup();
	return 0;
}
