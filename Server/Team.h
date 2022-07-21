#include <iostream>
#include <vector>
#include <algorithm>
#include <direct.h>
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
	string toString();
	void setName(string);
	static int readDb(vector<Team> &);
	static string addTeam(vector<Team> &, Team);
	static bool isExisted(vector<Team>, string);
};



