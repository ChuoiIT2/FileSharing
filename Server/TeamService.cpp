#include "TeamService.h"
/**
* Default contructor of TeamService
*/
TeamService::TeamService() {

}

/**
* Destructors of TeamService
*/
TeamService::~TeamService() {

}


/**
* @function readDb: read team data and store it to the current team list
* 
* @param teams: pointer to the current team list
* @return 0 if success, 1 if error
*/
int TeamService::readDb(vector<Team> &teams) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, DB_PATH.c_str(), "rt");
	if (error != 0) {
		return 1;
	}

	char buff[DB_BUFF];
	vector<Team> tmp;
	while (fgets(buff, DB_BUFF, fTeam) != NULL) {
		buff[strlen(buff) - 1] = 0;
		string name(buff);
		tmp.push_back(Team(name));
	}
	teams = tmp;

	fclose(fTeam);
	return 0;
}

/**
* @function isExisted: check if the team has been existed
* 
* @param teams: the current team list
* @param teamName: name of the team has to check
* @return true if team has exist, false if not
*/
bool TeamService::isExisted(vector<Team> teams, string teamName) {
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
* @function createTeam: handle request create team of user
* 
* @param userTeams: pointer to the list of user team
* @param teams: pointer to the list of team
* @param team: store the information of the team want to create
* @param username: name of user send request
* @return RES_ADDTEAM_EXISTED if the team exist, RES_ADDTEAM_SUCCESS if create team success
* RES_UNDEFINED_ERROR if error is undefined
*/
string TeamService::createTeam(vector<UserTeam> &userTeams, vector<Team> &teams, Team team, string username) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, DB_PATH.c_str(), "at");
	if (error != 0) {
		return RES_UNDEFINED_ERROR;
	}

	string teamName = team.getName();
	if (isExisted(teams, teamName)) {
		fclose(fTeam);
		return RES_ADDTEAM_EXISTED;
	}
	else {
		if (UserTeamService::createTeam(userTeams, teamName, username)) {
			fclose(fTeam);
			return RES_UNDEFINED_ERROR;
		}
		teams.push_back(team);
		string endLine = teamName + "\n";

		fwrite(endLine.c_str(), sizeof(char), endLine.length(), fTeam);
		fclose(fTeam);

		string path = ROOT_DATA_PATH + teamName;
		_mkdir(path.c_str());

		return RES_ADDTEAM_SUCCESS;
	}
}