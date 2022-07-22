// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <iostream>
#include <tchar.h>
#include "ws2tcpip.h"
#include "winsock2.h"
#include "process.h"
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define MAX_CLIENT 640
#define MAX_ACCOUNT 2050
#define MAX_THREAD 10

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

typedef struct Client {
	SOCKET socket;
	char clientIP[INET_ADDRSTRLEN];
	int  clientPort;
	char username[BUFF_SIZE];
} Client;


typedef struct Account {
	char userName[128];
	int status;
} Account;

typedef struct ThreadInfo {
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD nEvents;
} ThreadInfo;


// TODO: reference additional headers your program requires here
void InittiateWinsock();
sockaddr_in ConstructAddress(string IPAdd, int port);
int Receive(SOCKET s, char *buff, int size, int flags);
int Send(SOCKET s, char *buff, int size, int flags);
void cleanUp(int iThread, int index);
unsigned __stdcall worker(void *param);