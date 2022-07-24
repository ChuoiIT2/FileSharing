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
FILE* file;

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

void readDb() {
	if (UserService::readDb(users) ||
		TeamService::readDb(teams) ||
		UserTeamService::readDb(usersTeams)) {
		exit(1);
	}
}

int main(int argc, char** argv) {
	if (handleArguments(argc, argv) != 0) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}

	readDb();

	constructWinsock();
	for (int i = 0; i < MAX_THREAD; i++) {
		for (int j = 0; j < WSA_MAXIMUM_WAIT_EVENTS; j++) {
			clients[i][j].socket = 0;
			clients[i][j].username = "";
		}
		threads[i].nEvents = 0;
	}

	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr;
	serverAddr = constructAddr(SERVER_HOST, SERVER_PORT);

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot associate a local address with server socket");
		WSACleanup();
		return 0;
	}

	if (listen(listenSock, 10)) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot associate a local address with server socket");
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
			cout << "Accept incoming connection from [" << clientIP << ":" << clientPort << "]\n";
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

vector<string> splitDataStreaming(char* receiveData) {
	string dataSplit(receiveData + 4);

	string::size_type pos = dataSplit.find_first_of(" ");
	if (pos == string::npos) {
		return{ dataSplit, " " };
	}
	string methodRequest = dataSplit.substr(0, pos);
	string payload = dataSplit.substr(pos + 1);

	vector<string> result = { methodRequest, payload };

	return result;
}

int sReceive(SOCKET s, char* buff, int size = BUFF_SIZE, int flags = 0) {
	int ret = recv(s, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot receive data");
	}

	return ret;
}

/* The send() wrapper function*/
int sSend(SOCKET s, char* buff, int size, int flags = 0) {
	int ret = send(s, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot send data");
	}
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

string handleRequest(char* buff, Client &client) {
	vector<string> reqData = splitDataStreaming(buff);
	string method = reqData[0], payload = reqData[1];
	vector<string> detailPayload;

	if (method != REQ_UPLOADING) {
		detailPayload = Helpers::splitString(payload, ' ');
	}
	if (method == REQ_TEAMS || method == REQ_VIEW) {
		detailPayload.clear();
	}

	if (method == REQ_REGISTER) {
		// REGISTER[username][password]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}

		string result = UserService::registerAccount(users, { detailPayload[0], detailPayload[1] });
		if (result == RES_REGISTER_SUCCESS) {
			client.username = detailPayload[0];
			client.isLoggedIn = true;
		}
		return result;
	} else if (method == REQ_LOGIN) {
		// LOGIN [username] [password]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}

		string result = UserService::checkLogin(users, { detailPayload[0], detailPayload[1] });
		if (result == RES_LOGIN_SUCCESS) {
			client.username = detailPayload[0];
			client.isLoggedIn = true;
		}
		return result;
	} else if (method == REQ_ADDTEAM) {
		if (detailPayload.size() != 1) {
			// ADDTEAM [team_name]
			return RES_UNDEFINED_ERROR;
		}
		return TeamService::createTeam(usersTeams, teams, { detailPayload[0] }, client.username);
	} else if (method == REQ_JOIN) {
		// JOIN [team_name]
		if (detailPayload.size() != 1) {
			return RES_UNDEFINED_ERROR;
		}
		return UserTeamService::requestJoinTeam(usersTeams, teams, detailPayload[0], client.username);
	} else if (method == REQ_ACCEPT) {
		// ACCEPT [team_name] [username]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		return UserTeamService::acceptRequest(usersTeams, teams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_UPLOAD) {
		// UPLOAD [team_name] [remote_dir_path] [fileName]
		if (detailPayload.size() != 3) {
			return RES_UNDEFINED_ERROR;
		}
		return FileService::uploadFile(usersTeams, detailPayload[0], client.username, detailPayload[1], detailPayload[2]);
	} else if (method == REQ_UPLOADING) {
	} else if (method == REQ_RM) {
		// RM [team_name] [remote_file_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		return FileService::removeFile(usersTeams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_MKDIR) {
		// MKDIR [team_name] [path] [dir_name]
		if (detailPayload.size() != 3) {
			return RES_UNDEFINED_ERROR;
		}
		return FileService::createDir(usersTeams, detailPayload[0], client.username, detailPayload[1], detailPayload[2]);
	} else if (method == REQ_RMDIR) {
		// RMDIR [team_name] [remote_dir_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		return FileService::removeDir(usersTeams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_DOWNLOAD) {
		// DOWNLOAD [team_name] [remote_file_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		return FileService::downloadFile(usersTeams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_TEAMS) {
		// TEAMS
		if (detailPayload.size() != 0) {
			return RES_UNDEFINED_ERROR;
		}
		string result = UserTeamService::getTeamsOfUser(usersTeams, client.username, client.teams);
		for (auto teamName : client.teams) {
			result += " " + teamName;
		}
		return result;
	} else if (method == REQ_VIEW) {
		// VIEW [team_name]
		if (detailPayload.size() != 1) {
			return RES_UNDEFINED_ERROR;
		}
		string result = FileService::viewFileStructure(usersTeams, detailPayload[0], client.username, client.fileStructure);
		for (auto item : client.fileStructure) {
			result += " " + item;
		}
	} else {
		return RES_UNDEFINED_ERROR;
	}
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

			ret = sReceive(clients[iThread][index].socket, recvBuff, BUFF_SIZE);

			if (ret <= 0) {
				cleanUp(iThread, index);
				WSAResetEvent(threads[iThread].events[index]);
			} else {
				recvBuff[ret] = 0;
				memcpy(sendBuff, recvBuff, ret);

				string result = handleRequest(recvBuff, clients[iThread][index]);
				ret = sSend(clients[iThread][index].socket, (char*)result.c_str(), result.length());
				if (ret == SOCKET_ERROR) {
					cleanUp(iThread, index);
				}

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

int handleSaveFile(SOCKET clientSocket, string filePath) {
	errno_t error = fopen_s(&file, filePath.c_str(), "wb");

	if (error != 0) {
		cout << "Error: Cannot open file: " << filePath << endl;
		return 1;
	}

	char rBuff[BUFF_SIZE] = "", sDataLength[4] = "";
	int readBytes = 0, recvBytes = 0, dataLength = 0;
	do {
		recvBytes = sReceive(clientSocket, rBuff, BUFF_SIZE, 0);
		if (recvBytes == SOCKET_ERROR) {
			return 1;
		}
		vector<string> dataSplit = splitDataStreaming(rBuff);
		string payload = dataSplit[1];
		int payLoadLength = payload.length();

		memcpy_s(rBuff, payLoadLength, payload.c_str(), payLoadLength);
		memcpy_s(sDataLength, 4, rBuff, 4);

		int dataLength = Helpers::getLength(sDataLength);
		if (dataLength == 0) {
			fclose(file);
		} else {
			fwrite(rBuff + 4, sizeof(char), dataLength, file);
		}
	} while (dataLength != 0);

	return 0;
}

int handleSendFile(SOCKET clientSocket, string filePath) {
	errno_t error = fopen_s(&file, filePath.c_str(), "rb");

	if (error != 0) {
		cout << "Error: Cannot open file: " << filePath << endl;
		return 1;
	}

	char sBuff[BUFF_SIZE] = "";
	int sendBytes = 0, readBytes = 0, RES_DOWNLOADING_LEN = strlen(RES_DOWNLOADING);
	memcpy_s(sBuff, RES_DOWNLOADING_LEN, RES_DOWNLOADING, RES_DOWNLOADING_LEN);
	memcpy_s(sBuff + RES_DOWNLOADING_LEN, 1, " ", 1);

	cout << "-> Send file to client...";
	do {
		readBytes = fread(sBuff + RES_DOWNLOADING_LEN + 5, 1, BUFF_SIZE - (RES_DOWNLOADING_LEN + 5), file);
		if (readBytes == 0) {
			break;
		}

		const char* sLength = Helpers::convertLength(readBytes);
		memcpy_s(sBuff + RES_DOWNLOADING_LEN + 1, 4, sLength, 4);

		sendBytes = sSend(clientSocket, sBuff, RES_DOWNLOADING_LEN + 5 + readBytes, 0);
		if (sendBytes == SOCKET_ERROR) {
			return 1;
		}

		cout << ".";
	} while (readBytes > 0);

	const char* sLength = Helpers::convertLength(0);
	memcpy_s(sBuff + RES_DOWNLOADING_LEN + 1, 4, sLength, 4);
	sendBytes = sSend(clientSocket, sBuff, RES_DOWNLOADING_LEN + 5, 0);

	if (sendBytes == SOCKET_ERROR) {
		return 1;
	}

	fclose(file);

	return 0;
}
