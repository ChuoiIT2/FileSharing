#include "UserTeam.h"
#include ""

UserTeam::UserTeam(string _username, string _teamName, Role _role = MEMBER, UserTeamStatus _status = PENDING) {
	username = _username;
	teamName = _teamName;
	role = _role;
	status = _status;
}

UserTeam::~UserTeam() { }

string UserTeam::requestJoinTeam(string teamName, vector<UserTeam> usersTeams, User user) {
	// check if team exist, leave util Team is completed
	if (true) {
		FILE* fUserTeam;
		errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "at");
		if (error != 0) {
			cout << "Cannot open USER_TEAM_DB";

		}
	}
}