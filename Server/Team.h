#include <iostream>
#include <vector>

#pragma once

using namespace std;

class Team {
private:
	string name;

public:
	Team();
	Team(string _name);
	~Team();
	int readTeamDb(string, vector<Team>);
	static int createTeam(vector<Team>, Team);
	static string DB_PATH;
};



