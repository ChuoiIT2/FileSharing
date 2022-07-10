#include "UserTeam.h"

UserTeam::UserTeam(string _username, string _teamName, Role _role = MEMBER, UserTeamStatus _status = PENDING) {
	username = _username;
	teamName = _teamName;
	role = _role;
	status = _status;
}

UserTeam::~UserTeam() { }

json UserTeam::toJSON() {
	json j = json{ { "username", username }, {"teamName", teamName}, {"role", role }, {"status", status} };
	return j;
}

void UserTeam::fromJSON(const json& j) {
	username = j["username"].get<string>();
	teamName = j["teamName"].get<string>();
	role = j["role"].get<Role>();
	status = j["status"].get<UserTeamStatus>();
}