#include <iostream>
#include <vector>
#include <algorithm>

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
	string getName();
	void setName(string);
	int readTeamDb(vector<Team> &);
	static int createTeam(vector<Team> &, Team);
	static string DB_PATH;
};



