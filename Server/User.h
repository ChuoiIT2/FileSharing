#include <iostream>
#include "json.hpp"
#pragma once

using namespace std;
using json = nlohmann::json;

class User {
private:
	string username;
	string password;

public:
	User();
	User(string username, string password);
	~User();
	json toJSON();
	void fromJSON(const json &j);
};

