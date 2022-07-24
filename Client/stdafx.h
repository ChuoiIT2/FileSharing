// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define BUFF_SIZE 50000
#define TRANSFER_CODE 0
#define ERROR_CODE 1

struct Client {
	SOCKET socket;
	bool isLoggedIn = false;
	string username;
	string password;
	string downloadFileName;
	string uploadFileName;
	string uploadFilePath;
};

int handleArguments(int argc, char**argv);

//Screen
void mainScreen();
void authScreen();
void homeScreen();
void loginScreen();
void registerScreen();

// Handle functions
int handleAddTeam();
int handleJoinTeam();
int handleAccept();
int handleReqUpload();
int handleRm();
int handleMkdir();
int handleRmdir();
int handleReqDownload();
int handleGetTeams();
int handleViewFS();