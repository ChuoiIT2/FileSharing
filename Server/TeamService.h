#pragma once

#include <iostream>
#include <direct.h>
#include "Team.h"
#include "UserTeam.h"
#include "UserTeamService.h"
#include "protocols.h"
#include "Helpers.h"

class TeamService {
private:
public:
	TeamService();
	~TeamService();

	static string DB_PATH;

	static int readDb(vector<Team> &teams);
	static bool isExisted(vector<Team> teams, string teamName);
	static string createTeam(vector<UserTeam> &userTeams, vector<Team> &teams, Team team, string username);

};