#include <iostream>
#include "Team.h"

Team::Team() { }

Team::Team(string _name) {
	name = _name;
}

Team::~Team() { 
	name = "";
}

int Team::createTeam(string TEAM_DB_PATH, vector<Team> &teams, Team team) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, TEAM_DB_PATH.c_str(), "wb");
	if (error == 0) {
		cout << "Error: Cannot open TEAM_DB" << "\n";
		return 1;
	} else {
		teams.push_back(team);
		return 0;
	}
}
