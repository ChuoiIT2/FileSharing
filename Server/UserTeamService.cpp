#include "UserTeamService.h"

/**
* Default contructor of UserTeamService
*/
UserTeamService::UserTeamService() {

}

/**
* Destructors of UserTeamService
*/
UserTeamService::~UserTeamService() {

}

/**
* @function writeToDb: store user and user's team data to db
* 
* @param userTeams: the current user and user's team list
* @return 0 if success, 1 if error
*/
int UserTeamService::writeToDb(vector<UserTeam> usersTeams) {
	FILE* fUserTeam;
	errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "wt");
	if (error) {
		cout << "Error: Cannot open USER_TEAM_DB" << "\n";
		return 1;
	}

	string buff = "";
	for (auto userTeam : usersTeams) {
		buff += userTeam.getUsername() + " "
			+ userTeam.getTeamName() + " "
			+ to_string(userTeam.getRole()) + " "
			+ to_string(userTeam.getStatus())
			+ "\n";
	}
	fwrite(buff.c_str(), sizeof(char), buff.size(), fUserTeam);
	fclose(fUserTeam);

	return 0;
}

/**
* @function readDb: read user team data and store it to current user team list
*
* @param userTeams: pointer to the list of user team
* @return 0 if success, 1 if error
*/
int UserTeamService::readDb(vector<UserTeam> &usersTeams) {
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
		if (lines.size() != 4) {
			return 1;
		}
		Role role = stoi(lines[2]) == Role::OWNER ? Role::OWNER : Role::MEMBER;
		UserTeamStatus status = stoi(lines[3]) == UserTeamStatus::JOINED ? UserTeamStatus::JOINED : UserTeamStatus::PENDING;
		result.push_back(UserTeam(lines[0], lines[1], role, status));
	}
	usersTeams = result;
	fclose(fUserTeam);
	return 0;
}

/**
* @function isInTeam: check if user in the team
* 
* @param userTeams: current user team list
* @param teamName: name of team has to check
* @param username: name of user
* @return true if user is in team, false if not
*/
bool UserTeamService::isInTeam(vector<UserTeam> usersTeams, string teamName, string username) {
	for (auto userTeam : usersTeams) {
		if (userTeam.getTeamName() == teamName
			&& userTeam.getUsername() == username
			&& userTeam.getStatus() == UserTeamStatus::JOINED) {
			return true;
		}
	}

	return false;
}

/**
* @function isAdmin: check if user is the admin of team
*
* @param userTeams: current user team list
* @param teamName: name of team has to check
* @param username: name of user
* @return true if user is admin of the team, false if not
*/
bool UserTeamService::isAdmin(vector<UserTeam> usersTeams, string teamName, string username) {
	for (auto userTeam : usersTeams) {
		if (userTeam.getTeamName() == teamName
			&& userTeam.getUsername() == username
			&& userTeam.getStatus() == UserTeamStatus::JOINED
			&& userTeam.getRole() == Role::OWNER) {
			return true;
		}
	}

	return false;
}

/**
* @function isExistTeam: check if team has been existed
*
* @param teams: the current team list
* @param teamName: name of team has to check
*/
bool UserTeamService::isExistTeam(vector<Team> teams, string teamName) {
	auto teamChecking = std::find_if(teams.begin(), teams.end(), [teamName](Team team) {
		return team.getName() == teamName;
	});

	if (teamChecking != teams.end()) {
		return true;
	}
	else {
		return false;
	}
}

/**
* @function requestJoinTeam: handle request join team from user
*
* @param userTeams: the current user team list
* @param teams: the current team list
* @param teamName: name of the team has the request
* @param username: name of user
* @return RES_JOIN_ALREADY_IN if user has already in the team, RES_JOIN_SUCCESS if join team success,
* RES_JOIN_NOT_EXIST if no team exist, RES_UNDEFINED_ERROR if error is undefined
*/
string UserTeamService::requestJoinTeam(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string username) {
	if (isExistTeam(teams, teamName)) {
		for (auto userTeam : usersTeams) {
			if (userTeam.getUsername() == username && userTeam.getTeamName() == teamName) {
				if (userTeam.getStatus() == UserTeamStatus::JOINED) {
					return RES_JOIN_ALREADY_IN;
				}
				else {
					return RES_JOIN_SUCCESS;
				}
			}
		}

		FILE* fUserTeam;
		errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "at");
		if (error != 0) {
			cout << "Error: Cannot open USER_TEAM_DB" << endl;
			return RES_UNDEFINED_ERROR;
		}

		string buff = username + " "
			+ teamName + " "
			+ to_string(Role::MEMBER) + " "
			+ to_string(UserTeamStatus::PENDING)
			+ "\n";
		fwrite(buff.c_str(), sizeof(char), buff.size(), fUserTeam);
		fclose(fUserTeam);

		usersTeams.push_back(UserTeam(username, teamName));

		return RES_JOIN_SUCCESS;
	}
	else {
		return RES_JOIN_NOT_EXIST;
	}
}

/**
* @function acceptRequest: handle accept request join team of user
* 
* @param userTeams: the current user team list
* @param teams: the current team list
* @param teamName: name of the team has the request
* @param ownerUsename: owner of the team
* @param username: name of user has been accepted
* @return RES_FORBIDDEN_ERROR if wrong owner username, RES_ACCEPT_NO_REQUEST if has no request
* RES_ACCPEPT_SUCCESS if accept request join team, RES_UNDEFINED_ERROR if error is undefined
*/
string UserTeamService::acceptRequest(vector<UserTeam> &usersTeams, vector<Team> teams, string teamName, string ownerUsername, string username) {
	if (isExistTeam(teams, teamName)) {
		if (!isAdmin(usersTeams, teamName, ownerUsername)) {
			return RES_FORBIDDEN_ERROR;
		}
		bool hasRequest = false;
		int indexReq = -1;
		for (int i = 0, len = (int)usersTeams.size(); i < len; i++) {
			if (usersTeams[i].getTeamName() == teamName
				&& usersTeams[i].getUsername() == username
				&& usersTeams[i].getStatus() == UserTeamStatus::PENDING) {
				hasRequest = true;
				indexReq = i;
				break;
			}
		}

		if (!hasRequest) {
			return RES_ACCEPT_NO_REQUEST;
		}
		else {
			usersTeams[indexReq].setStatus(UserTeamStatus::JOINED);
			if (writeToDb(usersTeams) == 1) {
				return RES_UNDEFINED_ERROR;
			}
			return RES_ACCEPT_SUCCESS;
		}
	}
	else {
		return RES_ACCEPT_NO_REQUEST;
	}
}

/**
* @function createTeam: handle request create team from user
* 
* @param userTeams: the current user team list
* @param teams: the current team list
* @param username: name of user send request
* @return 0 if create team success, 1 if error
*/
int UserTeamService::createTeam(vector<UserTeam> &usersTeams, string teamName, string username) {
	FILE* fUserTeam;
	errno_t error = fopen_s(&fUserTeam, DB_PATH.c_str(), "at");
	if (error != 0) {
		cout << "Error: Cannot open USER_TEAM_DB" << endl;
		return 1;
	}
	string data = username + " "
		+ teamName + " "
		+ to_string(Role::OWNER) + " "
		+ to_string(UserTeamStatus::JOINED) + "\n";
	fwrite(data.c_str(), sizeof(char), data.length(), fUserTeam);
	usersTeams.push_back(UserTeam(username, teamName, Role::OWNER, UserTeamStatus::JOINED));

	fclose(fUserTeam);
	return 0;
}

/**
* @function getTeamOfUser: get the list team of user has been joined
* 
* @param userTeams: the current user team list
* @param teams: the current team list
* @param result: vector to store the result of the list team
* @return RES_TEAMS_SUCCESS
*/
string UserTeamService::getTeamsOfUser(vector<UserTeam> usersTeams, string username, vector<string> &result) {
	result.clear();
	for (auto userTeam : usersTeams) {
		if (userTeam.getUsername() == username && userTeam.getStatus() == UserTeamStatus::JOINED) {
			result.push_back(userTeam.getTeamName());
		}
	}
	return RES_TEAMS_SUCCESS;
}