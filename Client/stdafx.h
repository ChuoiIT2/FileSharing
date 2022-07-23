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
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define BUFF_SIZE 100000
#define TRANSFER_CODE 0
#define ERROR_CODE 1
// TODO: reference additional headers your program requires here

int handleArguments(int argc, char**argv);

//Screen
void mainScreen();
void authScreen();
void homeScreen();
void loginScreen();
void registerScreen();
void handleChoiceHomeScreen();

//hanlde user's selection
void handleCreateTeam();
void handleJoinTeam();
void handleUploadFile();
void handleDownloadFile();
void handleDeleteFile();
void handleCreateFolder();
void handleDeleteFolder();
void handleGetTeams();
void handleGetFileStructure();
void handleProcessTeamReq();
void handleLogout();
