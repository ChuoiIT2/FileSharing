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
long long nThread = -1; //index of thread

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

/**
* @function readDb: read database from files (storing accounts)
*
* @return void
*/
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
			long long iterThread = 0, iterClient = 0;
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

						EnterCriticalSection(&cs);
						clients[iterThread][iterClient] = clientTmp;
						LeaveCriticalSection(&cs);

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

/**
* @function constructWinsock: initial Winsock
*
* @return void
*/
void constructWinsock() {
	WSADATA wsaData;
	WORD vVersion = MAKEWORD(2, 2);

	if (WSAStartup(vVersion, &wsaData)) {
		cout << "Version is not supported\n";
		exit(0);
	}
}

/**
* @function constructAddr: constructor for server socket
* 
* @param ipAddr: ip address of server
* @param port: port of server 
* @return void
*/
sockaddr_in constructAddr(string ipAddr, int port) {
	const char* IPAddress = ipAddr.c_str();
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, IPAddress, &serverAddr.sin_addr);

	return serverAddr;
}

/**
* @function splitDataStreaming: handle streaming data sent by client
*
* @param receiveData: data from client
* @return vector which split data into two parts method request and payload
*/
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

/**
* @function sReceive: the receive wrapper function
*
* @param s: SOCKET for receiving data
* @param buff: buff for storing data received
* @param size: size of the buff, default is BUFF_SIZE
* @param flags: control flag, default is 0
* @return ret is number of bytes is received
*/
int sReceive(SOCKET s, char* buff, int size = BUFF_SIZE, int flags = 0) {
	int ret = recv(s, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot receive data");
	}

	return ret;
}

/**
*
* @function sSend: The send() wrapper function
* 
* @param s: SOCKET for sending data
* @param buff: buff for storing data sent
* @param size: size of the buff, default is BUFF_SIZE
* @param flags: control flag, default is 0
* @return ret is number of bytes is sent
*/
int sSend(SOCKET s, char* buff, int size, int flags = 0) {
	int ret = send(s, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot send data");
	}
	return ret;
}

/**
* @function cleanUp: clean up, close socket and envent
* 
* @param thread: index of thread
* @param index: index of client in thread
* @return void
*/
void cleanUp(long long iThread, int index) {
	closesocket(clients[iThread][index].socket);
	clients[iThread][index].reset();
	WSACloseEvent(threads[iThread].events[index]);

	//move clients from closed socket to the left
	//purpose of use: make end of array empty
	for (int i = index; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		clients[iThread][i].socket = clients[iThread][i + 1].socket;
		threads[iThread].events[i] = threads[iThread].events[i + 1];
		clients[iThread][i] = clients[iThread][i + 1];
	}
	clients[iThread][WSA_MAXIMUM_WAIT_EVENTS - 1].socket = 0;

	threads[iThread].nEvents--;
}

/**
* @function handleUploading: handle request uploading file from client
* 
* param buff: buff storing data received from client
* param client: struct type Client for storing client information
* @return string ""
*/
string handleUploading(char* buff, Client &client) {
	int length = Helpers::getLength(buff);
	int fLength = length - (int) strlen(REQ_UPLOADING) - 1;

	char fBuff[BUFF_SIZE];
	if (fLength > 0) {
		memcpy_s(fBuff, fLength, buff + 5 + strlen(REQ_UPLOADING), fLength);

		string fullPath = ROOT_DATA_PATH + client.curTeam + "/" + client.curDirPath + "/" + client.curFileName;
		if (!client.isOpeningFile) {
			errno_t error = fopen_s(&client.file, fullPath.c_str(), "wb");
			if (error) {
				cout << "error: " << error << endl;
			}
			client.isOpeningFile = true;
		}
		fwrite(fBuff, 1, fLength, client.file);
	} else {
		client.isOpeningFile = false;
		fclose(client.file);
	}
	return "";
}

/**
* @function handleDownloading: handle request downloading file from client
*
* param client: struct type Client for storign client information
* @return RES_UNDEFINED_ERROR if error is undefined, else return ""
*/
string handleDownloading(Client &client) {
	string fullPath = ROOT_DATA_PATH + client.curTeam + "/" + client.curFileName;

	if (!client.isOpeningFile) {
		errno_t error = fopen_s(&client.file, fullPath.c_str(), "rb");
		if (error) {
			return RES_UNDEFINED_ERROR;
		}
		client.isOpeningFile = true;
	}

	char fBuff[SEND_FILE_BUFF_SIZE] = "";
	const int RES_DOWNLOADING_LEN = (int) strlen(RES_DOWNLOADING);
	memcpy_s(fBuff, RES_DOWNLOADING_LEN, RES_DOWNLOADING, RES_DOWNLOADING_LEN);
	memcpy_s(fBuff + RES_DOWNLOADING_LEN, 1, " ", 1);

	int length = (int) fread(fBuff + RES_DOWNLOADING_LEN + 5, 1, SEND_FILE_BUFF_SIZE - (RES_DOWNLOADING_LEN + 5), client.file);
	memcpy_s(fBuff + RES_DOWNLOADING_LEN + 1, 4, Helpers::convertLength(length), 4);

	int ret = sSend(client.socket, fBuff, length + RES_DOWNLOADING_LEN + 5);

	if (length == 0) {
		fclose(client.file);
		client.isOpeningFile = false;
	}
	return "";
}

/**
* @function handleRequest: handle request received from client
*
* @param buff: buff for storing data received
* @param client: struct type Client for storign client information
* @return response of protocol from server
*/
string handleRequest(char* buff, Client &client) {
	vector<string> reqData = splitDataStreaming(buff);
	string method = reqData[0], payload = reqData[1];
	vector<string> detailPayload;

	if (method != REQ_UPLOADING) {
		detailPayload = Helpers::splitString(payload, ' ');
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
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return TeamService::createTeam(usersTeams, teams, { detailPayload[0] }, client.username);
	} else if (method == REQ_JOIN) {
		// JOIN [team_name]
		if (detailPayload.size() != 1) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return UserTeamService::requestJoinTeam(usersTeams, teams, detailPayload[0], client.username);
	} else if (method == REQ_ACCEPT) {
		// ACCEPT [team_name] [username]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return UserTeamService::acceptRequest(usersTeams, teams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_UPLOAD) {
		// UPLOAD [team_name] [remote_dir_path] [fileName]
		if (detailPayload.size() != 3) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return FileService::uploadFile(usersTeams, detailPayload[0], client, detailPayload[1], detailPayload[2]);
	} else if (method == REQ_UPLOADING) {
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return handleUploading(buff, client);
	} else if (method == REQ_RM) {
		// RM [team_name] [remote_file_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return FileService::removeFile(usersTeams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_MKDIR) {
		// MKDIR [team_name] [path] [dir_name]
		if (detailPayload.size() != 3) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return FileService::createDir(usersTeams, detailPayload[0], client.username, detailPayload[1], detailPayload[2]);
	} else if (method == REQ_RMDIR) {
		// RMDIR [team_name] [remote_dir_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		return FileService::removeDir(usersTeams, detailPayload[0], client.username, detailPayload[1]);
	} else if (method == REQ_DOWNLOAD) {
		// DOWNLOAD [team_name] [remote_file_path]
		if (detailPayload.size() != 2) {
			return RES_UNDEFINED_ERROR;
		}
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}

		return FileService::downloadFile(usersTeams, detailPayload[0], client, detailPayload[1]);
	} else if (method == REQ_DOWNLOADING) {
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}

		return handleDownloading(client);
	} else if (method == REQ_TEAMS) {
		// TEAMS
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
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
		if (!client.isLoggedIn) {
			return RES_UNAUTHORIZED_ERROR;
		}
		string result = FileService::viewFileStructure(usersTeams, detailPayload[0], client.username, client.fileStructure);
		for (auto item : client.fileStructure) {
			result += " " + item;
		}
		return result;
	} else {
		return RES_UNDEFINED_ERROR;
	}
}

/**
* @function worker: Thread for waiting network events on all socket, handle with each client
* 
* @param param: pointer passed from parent function
* @return 0
*/
unsigned __stdcall worker(void* param) {
	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];
	int ret;
	DWORD index;
	WSANETWORKEVENTS sockEvent;
	long long iThread = (long long)param;

	while (1) {
		memset(recvBuff, 0, sizeof(recvBuff));
		memset(sendBuff, 0, sizeof(sendBuff));

		index = WSAWaitForMultipleEvents(threads[iThread].nEvents, threads[iThread].events, FALSE, 30, FALSE);
		if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED) {
			continue;
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
				recvBuff[ret] = '\0';

				string result = handleRequest(recvBuff, clients[iThread][index]);
				if (!result.empty()) {
					ret = sSend(clients[iThread][index].socket, (char*)result.c_str(), (int) result.length());
					if (ret == SOCKET_ERROR) {
						cleanUp(iThread, index);
					}
				}

				WSAResetEvent(threads[iThread].events[index]);
			}

		}


		if (sockEvent.lNetworkEvents & FD_CLOSE) {

			if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				cout << "Client disconnected\n";
			}

			cleanUp(iThread, index);
			WSAResetEvent(threads[iThread].events[index]);
		}

	}

	return 0;
}

