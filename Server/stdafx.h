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

#define BUFF_SIZE 1400
#define SEND_FILE_BUFF_SIZE 2000
#define MAX_THREAD 10

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

struct Client {
	SOCKET socket;
	char ipAddr[INET_ADDRSTRLEN];
	int port;
	string username;
	vector<string> teams;
	vector<string> fileStructure;
	bool isLoggedIn;

	string curTeam;
	string curDirPath;
	string curFileName;
	FILE* file;
	bool isOpeningFile = false;
	int ret;

	void reset() {
		socket = 0;
		memset(ipAddr, 0, INET_ADDRSTRLEN);
		port = 0;
		username = "";
		teams = { };
		fileStructure = { };
		isLoggedIn = false;
		curTeam = "";
		curDirPath = "";
		curFileName = "";
		file = NULL;
		isOpeningFile = false;
		ret = 0;
	}
};

struct ThreadInfo {
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD nEvents;
};

void constructWinsock();
sockaddr_in constructAddr(string IPAdd, int port);
vector<string> splitDataStreaming(char* receiveData);
int sReceive(SOCKET s, char* buff, int size, int flags);
int sSend(SOCKET s, char* buff, int size, int flags);
void cleanUp(int iThread, int index);
unsigned __stdcall worker(void* param);
