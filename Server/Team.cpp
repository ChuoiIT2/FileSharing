#include <iostream>
#include "Team.h"

Team::Team() { }

Team::Team(string _name) {
	name = _name;
}

Team::~Team() { 
	name = "";
}


int Team::createTeam(vector<Team> teams, Team team) {
	FILE* fTeam;
	errno_t error = fopen_s(&fTeam, DB_PATH.c_str(), "at");
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







