#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "User.h"
#include "Team.h"
#include "UserTeam.h"
#include "Helpers.h"

using namespace std;

string SERVER_HOST;
int SERVER_PORT;
vector<User> users;
vector<Team> teams;
vector<UserTeam> usersTeams;
string User::DB_PATH, Team::DB_PATH, UserTeam::DB_PATH;

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
	User::DB_PATH = string(argv[3]);
	Team::DB_PATH = string(argv[4]);
	UserTeam::DB_PATH = string(argv[5]);
	return 0;
}

void testClasses() {
	User::readDb(users);
	Team::readDb(teams);
	UserTeam::readDb(usersTeams);

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
	cout << "admin-admin: " << User::checkLogin(users, User("admin", "admin")) << endl;
	cout << "wrong-wrong: " << User::checkLogin(users, User("wrong", "wrong")) << endl;

	cout << "\nTEST ADDTEAM:\n";
	cout << Team::addTeam(teams, Team("team2"));
}

int main(int argc, char** argv) {
	if (handleArguments(argc, argv) != 0) {
		cout << "Invalid arguments, please try again\n";
		return 0;
	}
	cout << "Server is running at " << SERVER_HOST << ":" << SERVER_PORT << "\n";

	testClasses();

	return 0;
}

