#include <iostream>
#include <vector>
#include "Helpers.h"

#pragma once

#define DB_BUFF 200000

using namespace std;

class User {
private:
	string username;
	string password;

public:
	User();
	User(string username, string password);
	~User();
	static int readUserDb(string USER_DB_PATH, vector<User> &users);
	static int registerAccount(string USER_DB_PATH, vector<User> &users, User user);
};

