#pragma once

#include <iostream>
#include <direct.h>
#include "Team.h"
#include "UserTeam.h"
#include "protocols.h"

class UserTeamService {
private:
public:
	UserTeamService();
	~UserTeamService();

	static string DB_PATH;

	static int writeToDb(vector<UserTeam> usersTeams);
	static int readDb(vector<UserTeam> &usersTeams);
	static bool isInTeam(vector<UserTeam> usersTeams, string teamName, string username);
	static bool isAdmin(vector<UserTeam> usersTeams, string teamName, string username);
	static bool isExistTeam(vector<Team> teams, string teamName);
	static string requestJoinTeam(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string username);
	static string acceptRequest(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string ownerUsername, string username);
	static int createTeam(vector<UserTeam> &usersTeams, string teamName, string username);
	static string getTeamsOfUser(vector<UserTeam> usersTeams, string username, vector<string> &result);
};