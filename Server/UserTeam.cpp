#include "UserTeam.h"

UserTeam::UserTeam(string _username, string _teamName, Role _role, UserTeamStatus _status) {
	username = _username;
	teamName = _teamName;
	role = _role;
	status = _status;
}

UserTeam::UserTeam(string _username, string _teamName) {
	username = _username;
	teamName = _teamName;
	role = MEMBER;
	status = PENDING;
}

UserTeam::~UserTeam() { }

string UserTeam::getUsername() {
	return username;
}

string UserTeam::getTeamName() {
	return teamName;
}

Role UserTeam::getRole() {
	return role;
}

UserTeamStatus UserTeam::getStatus() {
	return status;
}

void UserTeam::setStatus(UserTeamStatus _status) {
	status = _status;
}

string UserTeam::toString() {
	return username + " "
		+ teamName + " "
		+ (role == Role::OWNER ? "OWNER " : "MEMBER ")
		+ (status == UserTeamStatus::IN ? "IN" : "PENDING");
}
