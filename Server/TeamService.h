#pragma once

#include <iostream>
#include <direct.h>
#include "Team.h"
#include "UserTeam.h"
#include "UserTeamService.h"
#include "protocols.h"

class TeamService {
private:
public:
	TeamService() { }
	~TeamService() { }

	static string DB_PATH;

	static int readDb(vector<Team> &teams) {
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

	static bool isExisted(vector<Team> teams, string teamName) {
		auto teamChecking = std::find_if(teams.begin(), teams.end(), [teamName](Team team) {
			return team.getName() == teamName;
		});

		if (teamChecking != teams.end()) {
			return true;
		} else {
			return false;
		}
	}

	static string createTeam(vector<UserTeam> &userTeams, vector<Team> &teams, Team team, string username) {
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
			if (UserTeamService::createTeam(userTeams, teamName, username)) {
				return RES_UNDEFINED_ERROR;
			}
			string endLine = teamName + "\n";

			fwrite(endLine.c_str(), sizeof(char), endLine.length(), fTeam);
			fclose(fTeam);

			_mkdir(teamName.c_str());

			return RES_ADDTEAM_SUCCESS;
		}
	}

};