// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

string SERVER_HOST;
int SERVER_PORT;
string USER_DB_PATH, TEAM_DB_PATH, USER_TEAM_DB_PATH;

/**
* @function handleArguments: set port number for server from arguments user entered when start
*
* @param argc: number of argument
* @param argv: arguments demonstrated in string, each argument is splitted with space.
*
* @return 0 if missing or invalid arguments, 1 otherwise
*/
int handleArguments(int argc, char** argv) {
	if (argc != 5) {
		return 0;
	}
	char* stopString;
	SERVER_HOST = string(argv[1]);
	SERVER_PORT = strtoul(argv[1], &stopString, 10);
	USER_DB_PATH = string(argv[2]);
	TEAM_DB_PATH = string(argv[2]);
	USER_TEAM_DB_PATH = string(argv[2]);
	return 1;
}

int main(int argc, char** argv)
{
	if (!handleArguments(argc, argv)) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}

    return 0;
}

