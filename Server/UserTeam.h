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
	JOINED = 1
};

class UserTeam {
private:
	string username;
	string teamName;
	Role role;
	UserTeamStatus status;

public:
	UserTeam(string _username, string _teamName, Role _role, UserTeamStatus _status);
	UserTeam(string _username, string _teamName);
	~UserTeam();

	string getUsername();
	string getTeamName();
	Role getRole();
	UserTeamStatus getStatus();
	void setStatus(UserTeamStatus _status);
	string toString();
};

