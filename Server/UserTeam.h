#include <iostream>
#include <vector>
#include "Team.h"
#include "User.h"
#include "protocols.h"

#pragma once

using namespace std;

enum Role {
	OWNER = 0,
	MEMBER = 1
};

// Pending: user has requested to team, but haven't been in team 
// In: user is already in team
enum UserTeamStatus {
	PENDING = 0,
	IN = 1
};

class UserTeam {
private:
	string username;
	string teamName;
	Role role;
	UserTeamStatus status;

public:
	UserTeam(string _username, string _teamName, Role _role, UserTeamStatus _status);
	~UserTeam();
	static string DB_PATH;
	string toString();
	static int writeToDb(vector<UserTeam> usersTeams);
	static int readUserTeamDb(vector<UserTeam> &usersTeams);
	static bool isAdmin(vector<UserTeam> usersTeams, string teamName, string username);
	static string requestJoinTeam(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string username);
	static string acceptRequest(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string OwnerUsername, string username);
	static int createTeam(vector<UserTeam> &usersTeams, string teamName, string username);
};

