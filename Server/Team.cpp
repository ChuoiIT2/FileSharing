#include "Team.h"

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
		cout << "Error: Cannot open TEAM DB" << endl;
	}
	char buff[DB_BUFF];
	while (fgets(buff, DB_BUFF, fTeam) != NULL) {
		buff[strcspn(buff, "\n")] = 0;
		string name(buff);
		teams.push_back(Team(name));
	}
	fclose(fTeam);
}


int Team::createTeam(vector<Team> &teams, Team team) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, DB_PATH.c_str(), "at");
	string _teamName = team.getName();
	auto teamChecking = std::find_if(teams.begin(), teams.end(), [_teamName](Team tmp) {
		return tmp.getName() == _teamName;
	});

	if (teamChecking != teams.end()) {
		cout << "Team's name has already exists" << endl;
		return 1;
	}

	if (error != 0) {
		cout << "Error: Cannot open TEAM DB" << endl;
		return 1;
	} else {
		teams.push_back(team);
		string endLine = team.name;
		
		fwrite(endLine.c_str(), sizeof(char), endLine.length(), fTeam);
		fclose(fTeam);

		return 0;
	}
}







