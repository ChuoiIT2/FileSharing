#pragma once

#include "targetver.h"
#include <tchar.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <process.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#define BUFF_SIZE 2048
#define MAX_THREAD 10

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

struct Client {
	SOCKET socket;
	char ipAddr[INET_ADDRSTRLEN];
	int port;
	string username;
};

struct ThreadInfo {
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD nEvents;
};

void constructWinsock();
sockaddr_in constructAddr(string IPAdd, int port);
int sReceive(SOCKET s, char* buff, int size, int flags);
int sSend(SOCKET s, char* buff, int size, int flags);
void cleanUp(int iThread, int index);
unsigned __stdcall worker(void* param);