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
	bool isLoggedIn = false;
	string username;
	string password;
	string fileName = ""; //contain file's name need to send
	string responseFileName = ""; //file's name response
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
		}
		else {
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

	switch (select)
	{
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