// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Server/UserService.h"
#include "../Server/TeamService.h"
#include "../Server/UserTeamService.h"

string UserService::DB_PATH, TeamService::DB_PATH, UserTeamService::DB_PATH;

string SERVER_HOST;
int SERVER_PORT;
int choice = -1;
FILE* file;

// Global main Client variable
Client client;

int constructWinsock() {
	WSADATA wsaDATA;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaDATA)) {
		cout << "Error: Winsock 2.2 is not supported\n";
		return 1;
	}
	return 0;
}

int constructSocket() {
	client.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client.socket == INVALID_SOCKET) {
		cout << "Error " << WSAGetLastError() << ": Cannot create socket\n";
		return 1;
	}

	// Time out is 10s
	int tv = 10000;
	setsockopt(client.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	// Bind addrees to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_HOST.c_str(), &serverAddr.sin_addr);

	return 0;
}

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

int main(int argc, char**argv) {
	if (handleArguments(argc, argv) != 0) {
		return 0;
	}

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

int handleArguments(int argc, char**argv) {
	if (argc != 3) {
		cout << "Invalid list arguments\n";
		return 1;
	}

	SERVER_HOST = string(argv[1]);
	SERVER_PORT = Helpers::toInt(argv[2]);
	return 0;
}

int sReceive(char* buff, int size = BUFF_SIZE, int flags = 0) {
	int ret = recv(client.socket, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			cout << "Error: Time out for receiving data\n";
		} else {
			Helpers::printWSAError(WSAGetLastError(), "Cannot receive data");
		}
	}
	return ret;
}

int sSend(char* buff, int size, int flags = 0) {
	int ret = send(client.socket, buff, size, flags);
	if (ret == SOCKET_ERROR) {
		Helpers::printWSAError(WSAGetLastError(), "Cannot send data");
	}
	return ret;
}

char* streamingData(char* requestMethod) {
	int requestMethodLength = strlen(requestMethod);
	const char* sLength = Helpers::convertLength(requestMethodLength);
	int resLength = requestMethodLength + 4;

	char *result = new char[resLength];
	memcpy_s(result, 4, sLength, 4);
	memcpy_s(result + 4, requestMethodLength, requestMethod, requestMethodLength);
	result[resLength] = 0;

	return result;
}

int handleUpload(string filePath) {
	errno_t error = fopen_s(&file, filePath.c_str(), "rb");
	if (error) {
		cout << "Error: Cannot open file: " << filePath << endl;
		return 1;
	}

	char sBuff[BUFF_SIZE] = "";
	int REQ_UPLOADING_LEN = strlen(REQ_UPLOADING);
	memcpy_s(sBuff + 4, REQ_UPLOADING_LEN, REQ_UPLOADING, REQ_UPLOADING_LEN);
	memcpy_s(sBuff + 5, 1, " ", 1); // Add space after header

	int sentBytes = 0, length = 0, readBytes = 0;

	cout << "-> Uploading file to server...";
	do {
		// 5 is length of "length" + length of " "(space)
		readBytes = fread(sBuff + REQ_UPLOADING_LEN + 5, 1, BUFF_SIZE - (REQ_UPLOADING_LEN + 5), file);
		if (readBytes == 0) {
			break;
		}

		const char* sLength = Helpers::convertLength(readBytes);
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

	cout << "Uploaded file";
	fclose(file);

	return 0;
}

void mainScreen() {
	system("cls");
	cout << "__________File Sharing__________\n";
	while (!client.isLoggedIn) {
		authScreen();
	}
	homeScreen();
}

void authScreen() {
	system("cls");
	cout << "--> Hi, before use our service you need to log in first\n";
	cout << "--> Dont have any account? Register\n";
	cout << "\n*********************************************************\n";
	cout << "1. Login\n";
	cout << "2. Register\n";

	int option = -1;

	do {
		cout << ">Enter your selection: ";
		cin >> option;
	} while (option == 1 && option == 2);

	switch (option) {
		case 1:
			loginScreen();
			break;
		case 2:
			registerScreen();
			break;
	}

}

void homeScreen() {

	system("cls");
	cout << "\n-->Hi " << client.username << " !\n";
	cout << "1. Create team\n";
	cout << "2. Join team\n";
	cout << "3. Upload file\n";
	cout << "4. Download file\n";
	cout << "5. Delete file\n";
	cout << "6. Create folder\n";
	cout << "7. Delete folder\n";
	cout << "8. Get joined teams\n";
	cout << "9. Get file structure of team\n";
	cout << "10. Process join team request\n";
	cout << "11. Logout\n";

	handleChoiceHomeScreen();
}

void loginScreen() {
	cout << "\nTo login you need enter your name and your password!\n>Enter your username: ";
	cin >> client.username;
	cout << "\nEnter your password: ";
	cin >> client.password;
	client.isLoggedIn = true;
}

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
			client.isLoggedIn = true;
			valid = true;
		}

	}
}

void handleChoiceHomeScreen() {
	int select = -1;
	bool valid = false;

	while (!valid) {
		cout << "\n>Enter your selection: ";
		cin >> select;
		if (select < 1 || select > 11) {
			cout << "\n-->Invalid number please type again!\n";
			continue;
		}
		valid = true;
	}

	switch (select) {
		case 1:
			handleCreateTeam();
			break;
		case 2:
			handleJoinTeam();
			break;
		case 3:
			handleUploadFile();
			break;
		case 4:
			handleDownloadFile();
			break;
		case 5:
			handleDeleteFile();
			break;
		case 6:
			handleCreateFolder();
			break;
		case 7:
			handleDeleteFolder();
			break;
		case 8:
			handleGetTeams();
			break;
		case 9:
			handleGetFileStructure();
			break;
		case 10:
			handleProcessTeamReq();
			break;
		case 11:
			handleLogout();
			break;
	}
}

void handleCreateTeam() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleJoinTeam() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleUploadFile() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleDownloadFile() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleDeleteFile() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleCreateFolder() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleDeleteFolder() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleGetTeams() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleGetFileStructure() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleProcessTeamReq() {
	system("cls");
	cout << "\nHandle \n";
	_getch();
}

void handleLogout() {
	client.isLoggedIn = false;
	client.username = "";
	client.password = "";
}