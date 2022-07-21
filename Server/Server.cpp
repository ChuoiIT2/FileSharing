#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "User.h"
#include "Team.h"
#include "UserTeam.h"
#include "Helpers.h"
#include "UserService.h"
#include "TeamService.h"
#include "UserTeamService.h"
#include "FileService.h"

using namespace std;

string SERVER_HOST;
int SERVER_PORT;
vector<User> users;
vector<Team> teams;
vector<UserTeam> usersTeams;
string UserService::DB_PATH, TeamService::DB_PATH, UserTeamService::DB_PATH;

CRITICAL_SECTION cs;
Client Clients[MAX_THREAD][WSA_MAXIMUM_WAIT_EVENTS];
ThreadInfo threads[MAX_THREAD];
int nThread = -1; //index of thread

/**
* @function handleArguments: set port number for server from arguments user entered when start
*
* @param argc: number of argument
* @param argv: arguments demonstrated in string, each argument is splitted with space.
*
* @return 0 if missing or invalid arguments, 1 otherwise
*/
int handleArguments(int argc, char** argv) {
	if (argc != 6) {
		return 1;
	}
	SERVER_HOST = string(argv[1]);
	SERVER_PORT = Helpers::toInt(argv[2]);
	UserService::DB_PATH = string(argv[3]);
	TeamService::DB_PATH = string(argv[4]);
	UserTeamService::DB_PATH = string(argv[5]);
	return 0;
}

void testClasses() {
	UserService::readDb(users);
	TeamService::readDb(teams);
	UserTeamService::readDb(usersTeams);

	cout << "\nUSERS:\n";
	for (auto user : users) {
		cout << user.toString() << "\n";
	}
	cout << "\nTEAMS:\n";
	for (auto team : teams) {
		cout << team.toString() << "\n";
	}
	cout << "\nUSERS_TEAMS:\n";
	for (auto userTeam : usersTeams) {
		cout << userTeam.toString() << "\n";
	}

	cout << "\nTEST LOGIN:\n";
	cout << "admin-admin: " << UserService::checkLogin(users, User("admin", "admin")) << endl;
	cout << "wrong-wrong: " << UserService::checkLogin(users, User("wrong", "wrong")) << endl;

	cout << "\nTEST ADDTEAM:\n";
	cout << TeamService::createTeam(usersTeams, teams, Team("Team space"), "admin") << endl;

	cout << "\nTEST VIEW STRUCTURE:\n";
	vector<string> files;
	cout << FileService::viewFileStructure(usersTeams, "team1", "admin", files) << endl;
	for (auto file : files) {
		cout << file << "\n";
	}
}

int main(int argc, char** argv) {
	if (handleArguments(argc, argv) != 0) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}
	cout << "Server is running at " << SERVER_HOST << ":" << SERVER_PORT << "\n";

	testClasses();

	InittiateWinsock();
	for (int i = 0; i < MAX_THREAD; i++) {
		for (int j = 0; j < WSA_MAXIMUM_WAIT_EVENTS; j++) {
			Clients[i][j].socket = 0;
			strcpy_s(Clients[i][j].username, "");
		}
		threads[i].nEvents = 0;
	}

	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr;
	serverAddr = ConstructAddress(SERVER_ADDR, SERVER_PORT);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	printf("Server started at [%s:%d]\n", SERVER_ADDR, SERVER_PORT);

	SOCKET connSock;
	sockaddr_in clientAddr;
	char clientIP[INET_ADDRSTRLEN];
	int clientAddrLen = sizeof(clientAddr), clientPort;

	InitializeCriticalSection(&cs);
	while (1) {
		connSock = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR)
			printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);

			int m = 0, n = 0;
			bool isAllowNewSock = false;

			for (m = 0; m < MAX_THREAD; m++) {
				for (n = 0; n < WSA_MAXIMUM_WAIT_EVENTS; n++) {
					//critical section
					EnterCriticalSection(&cs);
					if (Clients[m][n].socket == 0) {
						LeaveCriticalSection(&cs);

						Client cTemp;
						cTemp.socket = connSock;
						strcpy_s(cTemp.clientIP, clientIP);
						cTemp.clientPort = clientPort;

						//critical section
						EnterCriticalSection(&cs);
						Clients[m][n] = cTemp;
						LeaveCriticalSection(&cs);

						//critical section
						EnterCriticalSection(&cs);
						threads[m].events[n] = WSACreateEvent();
						WSAEventSelect(Clients[m][n].socket, threads[m].events[n], FD_READ | FD_CLOSE);
						threads[m].nEvents++;
						LeaveCriticalSection(&cs);

						isAllowNewSock = true;
						break;
					}
				}
				EnterCriticalSection(&cs);

				if (m > nThread) {
					nThread = m;
					printf("Create new thread \n");
					_beginthreadex(0, 0, worker, (void *)nThread, 0, 0);
				}
				LeaveCriticalSection(&cs);
				if (isAllowNewSock) break;

			}

			if (m == MAX_THREAD) {
				printf("Error: Too many clients\n");
				closesocket(connSock);
			}

		}
	}
	DeleteCriticalSection(&cs);
	closesocket(listenSock);
	WSACleanup();

	return 0;
}

void InittiateWinsock() {
	WSADATA wsaData;
	WORD vVersion = MAKEWORD(2, 2);

	if (WSAStartup(vVersion, &wsaData)) {
		printf("Version is not supported\n");
		exit(0);
	}
}

sockaddr_in ConstructAddress(char* IPAdd, short port) {
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, IPAdd, &serverAddr.sin_addr);

	return serverAddr;
}

int Receive(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = recv(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error %d: Cannot receive data.\n", WSAGetLastError());
	else if (n == 0)
		printf("Client disconnects.\n");
	return n;
}

/* The send() wrapper function*/
int Send(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = send(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error %d: Cannot send data.\n", WSAGetLastError());

	return n;
}
void cleanUp(int iThread, int index) {
	closesocket(Clients[iThread][index].socket);
	Clients[iThread][index].socket = 0;
	WSACloseEvent(threads[iThread].events[index]);
	int i;
	for (i = index; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		Clients[iThread][i].socket = Clients[iThread][i + 1].socket;
		threads[iThread].events[i] = threads[iThread].events[i + 1];
		Clients[iThread][i] = Clients[iThread][i + 1];
	}
	Clients[iThread][WSA_MAXIMUM_WAIT_EVENTS - 1].socket = 0;

	threads[iThread].nEvents--;
}
unsigned __stdcall worker(void *param) {
	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];
	int ret;
	DWORD index;
	WSANETWORKEVENTS sockEvent;
	int iThread = (int)param;

	while (1) {
		//wait for network events on all socket in this thread

		memset(recvBuff, 0, sizeof(recvBuff));
		memset(sendBuff, 0, sizeof(sendBuff));

		index = WSAWaitForMultipleEvents(threads[iThread].nEvents, threads[iThread].events, FALSE, 30, FALSE);
		if (index == WSA_WAIT_TIMEOUT) continue;
		if (index == WSA_WAIT_FAILED) {
			printf("Error %d: WSAWaitForMultipleEvents() failed\n", WSAGetLastError());
			break;
		}

		index = index - WSA_WAIT_EVENT_0;
		WSAEnumNetworkEvents(Clients[iThread][index].socket, threads[iThread].events[index], &sockEvent);

		if (sockEvent.lNetworkEvents & FD_READ) {
			if (sockEvent.iErrorCode[FD_READ_BIT] != 0) {
				printf("FD_READ failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
				break;
			}

			ret = Receive(Clients[iThread][index].socket, recvBuff, BUFF_SIZE, 0);
			printf("Received %s\n", recvBuff);
			if (ret <= 0) {
				cleanUp(iThread, index);
				WSAResetEvent(threads[iThread].events[index]);

			}
			else {

				recvBuff[ret] = 0;
				memcpy(sendBuff, recvBuff, ret);
				Send(Clients[iThread][index].socket, sendBuff, strlen(sendBuff), 0);
				WSAResetEvent(threads[iThread].events[index]);
			}

		}


		if (sockEvent.lNetworkEvents & FD_CLOSE) {

			if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				printf("FD_CLOSE 2 failed with error %d\n", sockEvent.iErrorCode[FD_CLOSE_BIT]);
				//break;
			}

			cleanUp(iThread, index);
			WSAResetEvent(threads[iThread].events[index]);
		}

	}

	return 0;
}
