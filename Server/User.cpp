#include <iostream>
#include "User.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

User::User() { }

User::User(string _username, string _password) {
	username = _username;
	password = _password;
}

User::~User() { }

json User::toJSON() {
	json j = json{ { "username", username }, { "password", password } };
	return j;
}

void User::fromJSON(const json& j) {
	username = j["username"].get<string>();
	password = j["password"].get<string>();
}