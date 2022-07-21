#include <iostream>
#include <vector>
#include <algorithm>
#include "protocols.h"

#pragma once

using namespace std;

#define DB_BUFF 20000

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
	int readTeamDb(vector<Team> &);
	static string addTeam(vector<Team> &, Team);
	static bool isExisted(vector<Team>, string);
};



