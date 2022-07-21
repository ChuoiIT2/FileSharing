#include <iostream>
#include <vector>
#include "Team.h"
#include "User.h"

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
	static string requestJoinTeam(string teamName, vector<UserTeam> usersTeams, User user);
};

