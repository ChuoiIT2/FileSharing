#include <iostream>

#pragma once

using namespace std;

class Team {
private:
	string name;

public:
	Team();
	Team(string _name);
	~Team();

	string getName();
	string toString();
	void setName(string);
};



