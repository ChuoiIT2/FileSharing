// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "User.h"
#include "Team.h"
#include "UserTeam.h"

#define DB_BUFF 200000

using namespace std;

string SERVER_HOST;
int SERVER_PORT;
string USER_DB_PATH, TEAM_DB_PATH, USER_TEAM_DB_PATH;
FILE *fUser, *fTeam, *fUserTeam;
vector<User> users;
vector<Team> teams;
vector<UserTeam> userTeams;

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
		return 0;
	}
	char* stopString;
	SERVER_HOST = string(argv[1]);
	SERVER_PORT = strtoul(argv[2], &stopString, 10);
	USER_DB_PATH = string(argv[3]);
	TEAM_DB_PATH = string(argv[4]);
	USER_TEAM_DB_PATH = string(argv[5]);
	return 1;
}


int main(int argc, char** argv)
{
	if (!handleArguments(argc, argv)) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}
	cout << "Server is running at " << SERVER_HOST << ":" << SERVER_PORT << "\n";

    return 0;
}

