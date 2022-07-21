#include "UserTeam.h"

UserTeam::UserTeam(string _username, string _teamName, Role _role = MEMBER, UserTeamStatus _status = PENDING) {
	username = _username;
	teamName = _teamName;
	role = _role;
	status = _status;
}

UserTeam::~UserTeam() { }

string UserTeam::toString() {
	return username + " "
		+ teamName + " "
		+ (role == Role::OWNER ? "OWNER " : "MEMBER ")
		+ (status == UserTeamStatus::IN ? "IN" : "PENDING");
}

int UserTeam::writeToDb(vector<UserTeam> usersTeams) {
	FILE* fUserTeam;
	errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "wt");
	if (error) {
		cout << "Error: Cannot open USER_TEAM_DB" << "\n";
		return 1;
	}

	string buff = "";
	for (auto userTeam : usersTeams) {
		buff += userTeam.username + " "
			+ userTeam.teamName + " "
			+ to_string(userTeam.role) + " "
			+ to_string(userTeam.status)
			+ "\n";
	}
	fwrite(buff.c_str(), sizeof(char), buff.size(), fUserTeam);
	fclose(fUserTeam);

	return 0;
}

int UserTeam::readDb(vector<UserTeam> &usersTeams) {
	FILE* fUserTeam;
	errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "rt");
	if (error) {
		cout << "Error: Cannot open USER_TEAM_DB" << "\n";
		return 1;
	}

	char buff[DB_BUFF];
	vector<UserTeam> result;
	while (fgets(buff, DB_BUFF, fUserTeam) != NULL) {
		buff[strlen(buff) - 1] = 0;
		vector<string> lines = Helpers::splitString(string(buff), ' ');
		cout << buff << "\n";
		if (lines.size() != 4) {
			return 1;
		}
		Role role = stoi(lines[2]) == Role::OWNER ? Role::OWNER : Role::MEMBER;
		UserTeamStatus status = stoi(lines[3]) == UserTeamStatus::IN ? UserTeamStatus::IN : UserTeamStatus::PENDING;
		result.push_back(UserTeam(lines[0], lines[1], role, status));
	}
	usersTeams = result;

	fclose(fUserTeam);
	return 0;
}

bool UserTeam::isAdmin(vector<UserTeam> usersTeams, string teamName, string username) {
	for (auto userTeam : usersTeams) {
		if (userTeam.teamName == teamName
			&& userTeam.username == username
			&& userTeam.role == Role::OWNER) {
			return true;
		}
	}

	return false;
}

string UserTeam::requestJoinTeam(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string username) {
	// check if team exist, leave util Team is completed
	if (Team::isExisted(teams, teamName)) {
		// Check the request existant
		for (auto userTeam : usersTeams) {
			if (userTeam.username == username) {
				if (userTeam.status == UserTeamStatus::IN) {
					// Case already active in team
					return RES_JOIN_ALREADY_IN;
				} else {
					// Case the request is pending
					return RES_JOIN_SUCCESS;
				}
			}
		}

		FILE* fUserTeam;
		errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "at");
		if (error != 0) {
			cout << "Cannot open USER_TEAM_DB";
			return RES_UNDEFINED_ERROR;
		}

		string buff = username + " "
			+ teamName + " "
			+ to_string(Role::MEMBER) + " "
			+ to_string(UserTeamStatus::PENDING)
			+ "\n";
		fwrite(buff.c_str(), sizeof(char), buff.size(), fUserTeam);

		usersTeams.push_back(UserTeam(username, teamName));

		return RES_JOIN_SUCCESS;
	} else {
		return RES_JOIN_NOT_EXIST;
	}
}

string UserTeam::acceptRequest(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string ownerUsername, string username) {
	// check if team exist, leave util Team is completed
	if (Team::isExisted(teams, teamName)) {
		// Check isAdmin
		if (!isAdmin(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}
		bool hasRequest = false;
		int indexReq = -1;
		for (int i = 0, len = usersTeams.size(); i < len; i++) {
			if (usersTeams[i].teamName == teamName
				&& usersTeams[i].username == username
				&& usersTeams[i].status == UserTeamStatus::PENDING) {
				hasRequest = true;
				indexReq = i;
				break;
			}
		}

		if (!hasRequest) {
			return RES_ACCEPT_NO_REQUEST;
		} else {
			usersTeams[indexReq].status = UserTeamStatus::IN;
			if (writeToDb(usersTeams) == 1) {
				return RES_UNDEFINED_ERROR;
			}
			return RES_ACCEPT_SUCCESS;
		}
	} else {
		return RES_ACCEPT_NO_REQUEST;
	}
}

int UserTeam::createTeam(vector<UserTeam> &usersTeams, string teamName, string username) {
	FILE* fUserTeam;
	errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "at");
	if (error != 0) {
		cout << "Cannot open USER_TEAM_DB";
		return 1;
	}
	string data = username + " "
		+ teamName + " "
		+ to_string(Role::OWNER) + " "
		+ to_string(UserTeamStatus::IN) + "\n";
	fwrite(data.c_str(), sizeof(char), data.length(), fUserTeam);
	usersTeams.push_back(UserTeam(username, teamName, Role::MEMBER, UserTeamStatus::IN));

	fclose(fUserTeam);
	return 0;
}