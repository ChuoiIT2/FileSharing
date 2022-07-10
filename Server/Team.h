#include <iostream>
#include "json.hpp"
#pragma once

using namespace std;
using json = nlohmann::json;

class Team {
private:
	string name;

public:
	Team();
	Team(string _name);
	~Team();
	json toJSON();
	void fromJSON(const json &j);
};

