#include <iostream>
#include <vector>
#include <algorithm>
#include "protocols.h"

#pragma once

using namespace std;

#ifndef DB_BUFF
#define DB_BUFF 200000
#endif

class Team {
private:
	string name;

public:
	Team();
	Team(string _name);
	~Team();

	static string DB_PATH;

	string getName();
	void setName(string);
	static int readTeamDb(vector<Team> &);
	static string createTeam(vector<UserTeam> &, vector<Team> &, Team, string);
	static bool isExisted(vector<Team>, string);
};



