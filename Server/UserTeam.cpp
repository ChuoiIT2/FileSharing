#include "UserTeam.h"

UserTeam::UserTeam(string _username, string _teamName, Role _role = MEMBER, UserTeamStatus _status = PENDING) {
	username = _username;
	teamName = _teamName;
	role = _role;
	status = _status;
}

UserTeam::~UserTeam() { }
