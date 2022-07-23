#include "stdafx.h"
#include "User.h"
#include "Team.h"
#include "UserTeam.h"
#include "Helpers.h"
#include "UserService.h"
#include "TeamService.h"
#include "UserTeamService.h"
#include "FileService.h"

string SERVER_HOST;
int SERVER_PORT;
vector<User> users;
vector<Team> teams;
vector<UserTeam> usersTeams;
string UserService::DB_PATH, TeamService::DB_PATH, UserTeamService::DB_PATH;

CRITICAL_SECTION cs;
Client clients[MAX_THREAD][WSA_MAXIMUM_WAIT_EVENTS];
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

	cout << "\nTEST RMDIR:\n";
	cout << FileService::removeDir(usersTeams, "team1", "admin", "") << endl;
}

int main(int argc, char** argv) {
	if (handleArguments(argc, argv) != 0) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}

	constructWinsock();
	for (int i = 0; i < MAX_THREAD; i++) {
		for (int j = 0; j < WSA_MAXIMUM_WAIT_EVENTS; j++) {
			clients[i][j].socket = 0;
			strcpy_s(clients[i][j].username, "");
		}
		threads[i].nEvents = 0;
	}

	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr;
	serverAddr = constructAddr(SERVER_HOST, SERVER_PORT);

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		cout << "Error " << WSAGetLastError() << ": Cannot associate a local address with server socket" << endl;
		WSACleanup();
		return 0;
	}

	if (listen(listenSock, 10)) {
		cout << "Error " << WSAGetLastError() << ": Cannot associate a local address with server socket" << endl;
		WSACleanup();
		return 0;
	}

	cout << "Server is running at [" << SERVER_HOST << ":" << SERVER_PORT << "]\n";

	SOCKET connSock;
	sockaddr_in clientAddr;
	char clientIP[INET_ADDRSTRLEN];
	int clientAddrLen = sizeof(clientAddr), clientPort;

	InitializeCriticalSection(&cs);
	while (true) {
		connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR)
			cout << "Error " << WSAGetLastError() << ": Cannot permit incoming connection." << endl;
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			cout << "Accept incoming connection from " << clientIP << " : " << clientPort << endl;
			int iterThread = 0, iterClient = 0;
			//@iterThread: index of thread; @iterClient: index of client on thread

			bool isAllowNewSock = false;
			//@isAllowNewSock: check new connection is allow to connected

			for (iterThread = 0; iterThread < MAX_THREAD; iterThread++) {
				for (iterClient = 0; iterClient < WSA_MAXIMUM_WAIT_EVENTS; iterClient++) {
					//critical section
					EnterCriticalSection(&cs);
					if (clients[iterThread][iterClient].socket == 0) {
						LeaveCriticalSection(&cs);

						Client clientTmp;
						clientTmp.socket = connSock;
						strcpy_s(clientTmp.ipAddr, clientIP);
						clientTmp.port = clientPort;

						//critical section
						EnterCriticalSection(&cs);
						clients[iterThread][iterClient] = clientTmp;
						LeaveCriticalSection(&cs);

						//critical section
						EnterCriticalSection(&cs);
						threads[iterThread].events[iterClient] = WSACreateEvent();
						WSAEventSelect(clients[iterThread][iterClient].socket, threads[iterThread].events[iterClient], FD_READ | FD_CLOSE);
						threads[iterThread].nEvents++;
						LeaveCriticalSection(&cs);

						isAllowNewSock = true;
						break;
					}
				}
				EnterCriticalSection(&cs);

				if (iterThread > nThread) {
					nThread = iterThread;
					cout << "Create new thread\n";
					_beginthreadex(0, 0, worker, (void*)nThread, 0, 0);
				}
				LeaveCriticalSection(&cs);
				if (isAllowNewSock) {
					break;
				}

			}

			if (iterThread == MAX_THREAD) {
				cout << "Error: Too many clients.\n";
				closesocket(connSock);
			}

		}
	}
	DeleteCriticalSection(&cs);
	closesocket(listenSock);
	WSACleanup();

	return 0;
}

void constructWinsock() {
	WSADATA wsaData;
	WORD vVersion = MAKEWORD(2, 2);

	if (WSAStartup(vVersion, &wsaData)) {
		cout << "Version is not supported\n";
		exit(0);
	}
}

sockaddr_in constructAddr(string ipAddr, int port) {
	const char* IPAddress = ipAddr.c_str();
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, IPAddress, &serverAddr.sin_addr);

	return serverAddr;
}

int sReceive(SOCKET s, char* buff, int size, int flags) {
	int ret = recv(s, buff, size, flags);
	if (ret == SOCKET_ERROR)
		cout << "Error " << WSAGetLastError() << ": Cannot receive data.\n";
	else if (ret == 0)
		cout << "Client disconnects.\n";
	return ret;
}

/* The send() wrapper function*/
int sSend(SOCKET s, char* buff, int size, int flags) {
	int ret = send(s, buff, size, flags);
	if (ret == SOCKET_ERROR)
		cout << "Error " << WSAGetLastError() << ": Cannot send data.\n";
	return ret;
}

void cleanUp(int iThread, int index) {
	//close socket and close event
	closesocket(clients[iThread][index].socket);
	clients[iThread][index].socket = 0;
	WSACloseEvent(threads[iThread].events[index]);
	int i;

	//move clients from closed socket to the left
	//purpose of use: make end of array empty
	for (i = index; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		clients[iThread][i].socket = clients[iThread][i + 1].socket;
		threads[iThread].events[i] = threads[iThread].events[i + 1];
		clients[iThread][i] = clients[iThread][i + 1];
	}
	clients[iThread][WSA_MAXIMUM_WAIT_EVENTS - 1].socket = 0;

	threads[iThread].nEvents--;
}

unsigned __stdcall worker(void* param) {
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
			cout << "WSAWaitForMultipleEvents() failed\n";
			break;
		}

		index = index - WSA_WAIT_EVENT_0;
		WSAEnumNetworkEvents(clients[iThread][index].socket, threads[iThread].events[index], &sockEvent);

		if (sockEvent.lNetworkEvents & FD_READ) {
			if (sockEvent.iErrorCode[FD_READ_BIT] != 0) {
				cout << "FD_READ failed with error " << sockEvent.iErrorCode[FD_READ_BIT] << endl;
				break;
			}

			ret = Receive(clients[iThread][index].socket, recvBuff, BUFF_SIZE, 0);
			printf("Received %s\n", recvBuff);
			if (ret <= 0) {
				cleanUp(iThread, index);
				WSAResetEvent(threads[iThread].events[index]);

			} else {

				recvBuff[ret] = 0;
				memcpy(sendBuff, recvBuff, ret);
				Send(clients[iThread][index].socket, sendBuff, strlen(sendBuff), 0);
				WSAResetEvent(threads[iThread].events[index]);
			}

		}


		if (sockEvent.lNetworkEvents & FD_CLOSE) {

			if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				cout << "FD_READ failed with error " << sockEvent.iErrorCode[FD_READ_BIT] << endl;
				//break;
			}

			cleanUp(iThread, index);
			WSAResetEvent(threads[iThread].events[index]);
		}

	}

	return 0;
}
