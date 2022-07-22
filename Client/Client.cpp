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


struct Client {
	SOCKET socket;
	int key;
	int mode;
	bool isLoggedIn = false;
	string username;
	string password;
	char fileName[256] = ""; //contain file's name need to send
	char responseFileName[256] = ""; //file's name response
	FILE* ptrInput = NULL; //file pointer to file need to send
	FILE* ptrOutput = NULL; //file pointer to result file receive from server
};

Client client;

int main(int argc, char**argv)
{
	if (handleArguments(argc, argv) != 0) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}

	WSADATA wsaDATA;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaDATA)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Construct socket
	client.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client.socket == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.\n", WSAGetLastError());
		return 0;
	}

	int tv = 10000;
	setsockopt(client.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//bind addrees to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_HOST.c_str(), &serverAddr.sin_addr);

	//connect to server
	//if (connect(client.socket, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
	//	printf("Error %d: Cannot connect to server.\n", WSAGetLastError());
	//	return 0;
	//}

	cout << "Connected to server " << SERVER_HOST << ":" << SERVER_PORT << endl;
    
	while (1) {
		mainScreen();
	}
	return 0;
}

int handleArguments(int argc, char**argv) {
	if (argc != 3) {
		return 1;
	}

	SERVER_HOST = string(argv[1]);
	SERVER_PORT = Helpers::toInt(argv[2]);
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

	switch (option)
	{
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
	cout << "2. Logout\n";
	cout << "\nEnter your selection: ";
	cin >> choice;
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
		}
		else {
			client.isLoggedIn = true;
			valid = true;
		}
	}
}