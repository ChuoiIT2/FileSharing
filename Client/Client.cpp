// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Server/UserService.h"
#include "../Server/TeamService.h"
#include "../Server/UserTeamService.h"

string UserService::DB_PATH, TeamService::DB_PATH, UserTeamService::DB_PATH;

string SERVER_HOST;
int SERVER_PORT;
FILE* file;
map<string, string> resToMsg;

// Global main Client variable
Client client;

/**
* @function initMapResToMsg: map received response_code to response_message
*
* @return void;
*/
void initMapResToMsg() {
	resToMsg.insert(pair<string, string>(RES_REGISTER_SUCCESS, MSG_REGISTER_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_REGISTER_ACCOUNT_EXIST, MSG_REGISTER_ACCOUNT_EXIST));
	resToMsg.insert(pair<string, string>(RES_LOGIN_SUCCESS, MSG_LOGIN_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_LOGIN_WRONG_ACCOUNT, MSG_LOGIN_WRONG_ACCOUNT));
	resToMsg.insert(pair<string, string>(RES_ADDTEAM_SUCCESS, MSG_ADDTEAM_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_ADDTEAM_EXISTED, MSG_ADDTEAM_EXISTED));
	resToMsg.insert(pair<string, string>(RES_JOIN_SUCCESS, MSG_JOIN_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_JOIN_NOT_EXIST, MSG_JOIN_NOT_EXIST));
	resToMsg.insert(pair<string, string>(RES_JOIN_ALREADY_IN, MSG_JOIN_ALREADY_IN));
	resToMsg.insert(pair<string, string>(RES_ACCEPT_SUCCESS, MSG_ACCEPT_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_ACCEPT_NO_REQUEST, MSG_ACCEPT_NO_REQUEST));
	resToMsg.insert(pair<string, string>(RES_REQ_UPLOAD_SUCCESS, MSG_REQ_UPLOAD_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_UPLOAD_INVALID_REMOTE_PATH, MSG_UPLOAD_INVALID_REMOTE_PATH));
	resToMsg.insert(pair<string, string>(RES_UPLOAD_FILE_EXIST, MSG_UPLOAD_FILE_EXIST));
	resToMsg.insert(pair<string, string>(RES_UPLOAD_SUCCESS, MSG_UPLOAD_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_RM_SUCCESS, MSG_RM_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_RM_INVALID_REMOTE_PATH, MSG_RM_INVALID_REMOTE_PATH));
	resToMsg.insert(pair<string, string>(RES_MKDIR_SUCCESS, MSG_MKDIR_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_MKDIR_INVALID_PATH, MSG_MKDIR_INVALID_PATH));
	resToMsg.insert(pair<string, string>(RES_MKDIR_DIR_EXIST, MSG_MKDIR_DIR_EXIST));
	resToMsg.insert(pair<string, string>(RES_RMDIR_SUCCESS, MSG_RMDIR_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_RMDIR_INVALID_PATH, MSG_RMDIR_INVALID_PATH));
	resToMsg.insert(pair<string, string>(RES_REQ_DOWNLOAD_SUCCESS, MSG_REQ_DOWNLOAD_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_DOWNLOAD_INVALID_PATH, MSG_DOWNLOAD_INVALID_PATH));
	resToMsg.insert(pair<string, string>(RES_DOWNLOADING, MSG_DOWNLOADING));
	resToMsg.insert(pair<string, string>(RES_TEAMS_SUCCESS, MSG_TEAMS_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_VIEW_SUCCESS, MSG_VIEW_SUCCESS));
	resToMsg.insert(pair<string, string>(RES_UNDEFINED_ERROR, MSG_UNDEFINED_ERROR));
	resToMsg.insert(pair<string, string>(RES_UNAUTHORIZED_ERROR, MSG_UNAUTHORIZED_ERROR));
	resToMsg.insert(pair<string, string>(RES_FORBIDDEN_ERROR, MSG_FORBIDDEN_ERROR));
}

/**
* @function constructWinsock: initial winsock;
*
* @return 0 if success, 1 if error;
*/
int constructWinsock() {
	WSADATA wsaDATA;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaDATA)) {
		cout << "Error: Winsock 2.2 is not supported\n";
		return 1;
	}
	return 0;
}

/**
* @function constructSocket: construct client socket
*
* @return 0 if success, 1 if error;
*/
int constructSocket() {
	client.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client.socket == INVALID_SOCKET) {
		cout << "Error " << WSAGetLastError() << ": Cannot create socket\n";
		return 1;
	}

	// Time out is 10s
	int tv = 10000;
	setsockopt(client.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	return 0;
}

/**
* @function connectToServer: construct server address and connect
*
* @return 0 if success, 1 if error;
*/
int connectToServer() {
	// Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_HOST.c_str(), &serverAddr.sin_addr);

	// Request to connect server
	if (connect(client.socket, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot connect to server");
		closesocket(client.socket);
		WSACleanup();
		return 1;
	}
	cout << "Connected to server [" << SERVER_HOST << ":" << SERVER_PORT << "]\n";
	return 0;
}

/**
* @function sReceive: receive wrapper function
*
* @param buff: receive buff
*
* @param size: buff size
*
* @param flags: flag option when receive
* @return 0;
*/
int sReceive(char* buff, int size = BUFF_SIZE, int flags = 0) {
	int ret = recv(client.socket, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			cout << "Error: Time out for receiving data\n";
		} else {
			Helpers::printWSAError(WSAGetLastError(), "Cannot receive data");
		}
	}
	buff[ret] = 0;
	return ret;
}

/**
* @function sSend: send wrapper function
*
* @param buff: send buff
*
* @param size: buff size
*
* @param flags: flag option when send
* @return 0;
*/
int sSend(char* buff, int size, int flags = 0) {
	int ret = send(client.socket, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot send data");
	}
	return ret;
}

int main(int argc, char**argv) {
	if (handleArguments(argc, argv) != 0) {
		return 0;
	}

	initMapResToMsg();

	if (constructWinsock() != 0) {
		return 0;
	}

	if (constructSocket() != 0) {
		return 0;
	}

	if (connectToServer() != 0) {
		return 0;
	}

	while (true) {
		mainScreen();
	}

	return 0;
}

/**
* @function handleArguments: set port number for server from arguments user entered when start
*
* @param argc: number of argument
* @param argv: arguments demonstrated in string, each argument is splitted with space.
*
* @return 0 if missing or invalid arguments, 1 otherwise
*/
int handleArguments(int argc, char**argv) {
	if (argc != 3) {
		cout << "Invalid list arguments\n";
		return 1;
	}

	SERVER_HOST = string(argv[1]);
	SERVER_PORT = Helpers::toInt(argv[2]);
	return 0;
}

/**
* @function handleUpload: upload file after received allow response from server
*
* @param filepath: path of file to upload
*
* @return 0 if success, 1 otherwise
*/
int handleUpload(string filePath) {
	errno_t error = fopen_s(&file, filePath.c_str(), "rb");
	if (error) {
		cout << "Error: Cannot open file: " << filePath << endl;
		return 1;
	}

	char sBuff[BUFF_SIZE] = "";
	int REQ_UPLOADING_LEN = strlen(REQ_UPLOADING);
	memcpy_s(sBuff + 4, REQ_UPLOADING_LEN, REQ_UPLOADING, REQ_UPLOADING_LEN);
	memcpy_s(sBuff + 4 + REQ_UPLOADING_LEN, 1, " ", 1); // Add space after header

	int sentBytes = 0, length = 0, readBytes = 0;
	cout << "-> Uploading file to server...";
	do {
		// 5 is length of "length" + length of " "(space)
		readBytes = fread(sBuff + REQ_UPLOADING_LEN + 5, 1, BUFF_SIZE - (REQ_UPLOADING_LEN + 5), file);
		if (readBytes == 0) {
			break;
		}

		const char* sLength = Helpers::convertLength(readBytes + REQ_UPLOADING_LEN + 1);
		memcpy_s(sBuff, 4, sLength, 4);

		sentBytes = sSend(sBuff, REQ_UPLOADING_LEN + 5 + readBytes);
		if (sentBytes == SOCKET_ERROR) {
			return 1;
		}
		Sleep(50);
		cout << ".";
	} while (readBytes > 0);

	// Complete transfer file
	const char* sLength = Helpers::convertLength(0);
	memcpy_s(sBuff, 4, sLength, 4);
	sentBytes = sSend(sBuff, REQ_UPLOADING_LEN + 5);
	if (sentBytes == SOCKET_ERROR) {
		return 1;
	}
	cout << "\nComplete!\n";
	fclose(file);

	return 0;
}

/**
* @function handleDownload: download file after received allow response from server
*
* @param filePath: file's name to save
*
* @return 0 if success, 1 otherwise
*/
int handleDownload(string filePath) {
	errno_t error = fopen_s(&file, filePath.c_str(), "wb");
	if (error) {
		cout << "Error: Cannot open file: " << filePath << endl;
		return 1;
	}

	char rBuff[RECV_FILE_BUFF_SIZE] = "";
	int length = 0, ret = 0;
	const int RES_METHOD_LEN = strlen(RES_DOWNLOADING);
	do {
		ret = sReceive(rBuff);
		if (ret <= 0) {
			return 1;
		}
		cout << "-->Received " << ret << " bytes\n";
		//msg: "102" + " " + "4byte length" + data;
		length = Helpers::getLength(rBuff + RES_METHOD_LEN + 1);
		if (length == 0) {
			fclose(file);
		} else {
			fwrite(rBuff + 5 + RES_METHOD_LEN, 1, length, file);
		}
	} while (length != 0);

	return 0;
}

/**
* @function handleResponse: print response message and handle some special requestType
*
* @param requestType: type of request send to server
* @param res: code response from server
*
* @return void
*/
void handleResponse(string requestType, string res) {
	string msg = resToMsg[res];
	if (requestType == REQ_REGISTER) {
		if (res == RES_REGISTER_SUCCESS) {
			client.isLoggedIn = true;
		}
	} else if (requestType == REQ_LOGIN) {
		if (res == RES_LOGIN_SUCCESS) {
			client.isLoggedIn = true;
		}
	} else if (requestType == REQ_DOWNLOAD) {
		if (res == RES_REQ_DOWNLOAD_SUCCESS) {
			if (handleDownload(client.downloadFileName)) {
				msg = "Error while downloading file";
			} else {
				msg = "Download file completed";
			}
		}
	} else if (requestType == REQ_UPLOAD) {
		if (res == RES_REQ_UPLOAD_SUCCESS) {
			if (handleUpload(client.uploadFilePath)) {
				msg = "Error while uploading file";
			} else {
				msg = "Upload file completed";
			}
		}
	} else if (requestType == REQ_TEAMS) {
		if (res.find(RES_TEAMS_SUCCESS) != string::npos) {
			vector<string> teams = Helpers::splitString(res, ' ');
			msg = "List team:\n";
			for (int i = 1; i < teams.size(); i++) {
				msg += teams[i] + "\n";
			}
		}
	} else if (requestType == REQ_VIEW) {
		if (res.find(RES_VIEW_SUCCESS) != string::npos) {
			vector<string> data = Helpers::splitString(res, ' ');
			msg = "File structure:\n";
			for (int i = 1; i < data.size(); i++) {
				msg += data[i] + "\n";
			}
		}
	}

	cout << "* " << msg << endl;
}

/**
* @function sendAndReceive: send request with byte stream and receive response
*
* @param requestType: type of request
* @param data: data to send
*
* @return 0;
*/
int sendAndReceive(string requestType, vector<string> data) {
	char buff[BUFF_SIZE];
	string dataAfterReq = "";
	for (auto str : data) {
		dataAfterReq += (" " + str);
	}
	string payload = requestType;
	payload += dataAfterReq;

	int length = payload.length();
	const char* sLength = Helpers::convertLength(length);

	memcpy_s(buff, 4, sLength, 4);
	memcpy_s(buff + 4, length, payload.c_str(), length);

	// Send data
	int ret = sSend(buff, length + 4);
	if (ret <= 0) {
		exit(0);
	}

	// Receive data
	ret = sReceive(buff, BUFF_SIZE);
	if (ret <= 0) {
		exit(0);
	}
	handleResponse(requestType, string(buff));
	cout << "-->Press any key to continue\n";
	_getch();
	return 0;
}

/**
* @function mainScreen: show main screen
*
* @return void;
*/
void mainScreen() {
	system("cls");
	cout << "__________File Sharing__________\n";
	while (!client.isLoggedIn) {
		authScreen();
	}
	homeScreen();
}

/**
* @function authScreen: screen appear when user is not login
*
* @return void;
*/
void authScreen() {
	system("cls");
	cout << "--> Hi, before use our service you need to log in first\n";
	cout << "--> Dont have any account? Register\n";
	cout << "\n*********************************************************\n";
	cout << "1. Login\n";
	cout << "2. Register\n";

	string line;
	int iOption = -1;
	bool valid = false;

	while (!valid) {
		cout << "\n>Enter your selection: ";
		getline(cin, line);
		cin.clear();
		iOption = line[0] - '0';
		if (iOption < 1 || iOption > 2 || line.size() > 1) {
			cout << "\n-->Invalid option, please type again!\n";
			iOption = -1;
			continue;
		}
		valid = true;
	}

	switch (iOption) {
		case 1:
			loginScreen();
			break;
		case 2:
			registerScreen();
			break;
	}

}

/**
* @function loginScreen: screen appear when user choose login
*
* @return void;
*/
void loginScreen() {
	cout << "\nTo login you need enter your name and your password!\n>Enter your username: ";
	cin >> client.username;
	cout << "\nEnter your password: ";
	cin >> client.password;

	sendAndReceive(REQ_LOGIN, { client.username, client.password });
}

/**
* @function registerScreen: screen appear when user choose resgister
*
* @return void;
*/
void registerScreen() {
	bool valid = false;

	while (!valid) {
		cout << "\nTo register you need enter your name and your password!\n>Enter your username: ";
		cin >> client.username;
		cout << "\n>Enter your password: ";
		cin >> client.password;
		string confirmPassword;
		cout << "\n>Enter your password again: ";
		cin >> confirmPassword;

		if (client.password.compare(confirmPassword)) {
			cout << "\nYour password not match, please register again!\n";
			_getch();
			continue;
		} else {
			sendAndReceive(REQ_REGISTER, { client.username, client.password });
			break;
		}
	}
}

/**
* @function handleAddTeam: handle option create a team
*
* @return 0;
*/
int handleAddTeam() {
	string teamName;
	cout << "Enter team name to create: ";
	cin >> teamName;

	return sendAndReceive(REQ_ADDTEAM, { teamName });
}

/**
* @function handleJoinTeam: handle option join a team
*
* @return 0;
*/
int handleJoinTeam() {
	string teamName;
	cout << "Enter team name to join: ";
	cin >> teamName;

	return sendAndReceive(REQ_JOIN, { teamName });
}

/**
* @function handleAccept: handle option accept join team request
*
* @return 0;
*/
int handleAccept() {
	string teamName, username;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter username for accept: ";
	cin >> username;

	return sendAndReceive(REQ_ACCEPT, { teamName, username });
}

/**
* @function handleReqUpload: handle option upload a file
*
* @return 0;
*/
int handleReqUpload() {
	string teamName, remoteDirPath;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter directory path at remote storage:\n";
	cin >> remoteDirPath;
	cout << "Enter file path at local storage to upload:\n";
	cin >> client.uploadFilePath;
	cout << "Enter file's name to save on server:\n";
	cin >> client.uploadFileName;

	return sendAndReceive(REQ_UPLOAD, { teamName, remoteDirPath, client.uploadFileName });
}

/**
* @function handleRm: handle option remove a file
*
* @return 0;
*/
int handleRm() {
	string teamName, remoteFilePath;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter file path at remote storage:\n";
	cin >> remoteFilePath;

	return sendAndReceive(REQ_RM, { teamName, remoteFilePath });
}

/**
* @function handleReqUpload: handle option create a new directory
*
* @return 0;
*/
int handleMkdir() {
	string teamName, remoteDirPath, dirName;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter directory path at remote for locating new sub directory:\n";
	cin >> remoteDirPath;
	cout << "Enter directory name to create: ";
	cin >> dirName;

	return sendAndReceive(REQ_MKDIR, { teamName, remoteDirPath, dirName });
}

/**
* @function handleRmdir: handle option remove a directory
*
* @return 0;
*/
int handleRmdir() {
	string teamName, remoteDirPath;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter directory path at remote storage:\n";
	cin >> remoteDirPath;

	return sendAndReceive(REQ_RMDIR, { teamName, remoteDirPath });
}

/**
* @function handleReqDownload: handle option download a file
*
* @return 0;
*/
int handleReqDownload() {
	string teamName, remoteFilePath;
	cout << "Enter team name: ";
	cin >> teamName;
	cout << "Enter file path at remote storage:\n";
	cin >> remoteFilePath;
	cout << "Enter file's name to save:\n";
	cin >> client.downloadFileName;

	return sendAndReceive(REQ_DOWNLOAD, { teamName, remoteFilePath });
}

/**
* @function handleGetTeams: handle option get a list of joined Teams
*
* @return 0;
*/
int handleGetTeams() {
	return sendAndReceive(REQ_TEAMS, { });
}

/**
* @function handleViewFS: handle option get file structure of a team
*
* @return 0;
*/
int handleViewFS() {
	string teamName;
	cout << "Enter team name: ";
	cin >> teamName;

	return sendAndReceive(REQ_VIEW, { teamName });
}

/**
* @function homeScreen: show home screen when user logged in
*
* @return void;
*/
void homeScreen() {
	system("cls");
	cout << "\n-->Hi " << client.username << " !\n";
	cout << "1. Create team\n";
	cout << "2. Join team\n";
	cout << "3. Upload file\n";
	cout << "4. Download file\n";
	cout << "5. Delete file\n";
	cout << "6. Create sub directory\n";
	cout << "7. Delete a directory\n";
	cout << "8. Get joined teams\n";
	cout << "9. Get file structure of team\n";
	cout << "10. Process join team request\n";
	cout << "11. Logout\n";

	string line;
	int iOption = -1;
	bool valid = false;

	while (!valid) {
		cout << "\n>Enter your selection: ";
		cin.ignore();
		getline(cin, line);
		cin.clear();

		if (line.size() == 1) {
			iOption = line[0] - '0';
		}

		if (line.size() == 2) {
			iOption = (line[0] - '0') * 10 + (line[1] - '0');
		}

		if (iOption < 1 || iOption > 11) {
			cout << "\n-->Invalid option, please type again!\n";
			iOption = -1;
			continue;
		}
		valid = true;
	}

	switch (iOption) {
		case 1:
			handleAddTeam();
			break;
		case 2:
			handleJoinTeam();
			break;
		case 3:
			handleReqUpload();
			break;
		case 4:
			handleReqDownload();
			break;
		case 5:
			handleRm();
			break;
		case 6:
			handleMkdir();
			break;
		case 7:
			handleRmdir();
			break;
		case 8:
			handleGetTeams();
			break;
		case 9:
			handleViewFS();
			break;
		case 10:
			handleAccept();
			break;
		case 11:
			client.isLoggedIn = false;
			break;
		default:
			break;
	}
}
