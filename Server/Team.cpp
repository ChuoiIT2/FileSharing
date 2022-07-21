#include "Team.h"
#include "User.h"
#include "UserTeam.h"

Team::Team() { }

Team::Team(string _name) {
	name = _name;
}

Team::~Team() { 
	name = "";
}

string Team::getName() {
	return name;
}

void Team::setName(string _name) {
	name = _name;
}

int Team::readTeamDb(vector<Team> &teams) {
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


string Team::createTeam(vector<UserTeam> &userTeams, vector<Team> &teams, Team team, string userName) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, DB_PATH.c_str(), "at");
	if (error != 0) {
		return RES_UNDEFINED_ERROR;
	} 

	string teamName = team.getName();
	if (isExisted(teams, teamName)) {
		return RES_ADDTEAM_EXISTED;
	} else {
		teams.push_back(team);
		if (UserTeam::createTeam(userTeams, teamName, userName)) {
			return RES_UNDEFINED_ERROR;
		}
		string endLine = teamName + "\n";
		
		fwrite(endLine.c_str(), sizeof(char), endLine.length(), fTeam);
		fclose(fTeam);

		return RES_ADDTEAM_SUCCESS;
	}
}

bool Team::isExisted(vector<Team> teams, string teamName) {
	auto teamChecking = std::find_if(teams.begin(), teams.end(), [teamName](Team team) {
		return team.getName() == teamName;
	});

	if (teamChecking != teams.end()) {
		return 1;
	} else {
		return 0;
	}
}