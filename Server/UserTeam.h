#include <iostream>

#pragma once

using namespace std;

enum Role {
	OWNER,
	MEMBER
};

// Pending: user has requested to team, but haven't been in team 
// In: user is already in team
enum UserTeamStatus {
	PENDING,
	IN
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
};

