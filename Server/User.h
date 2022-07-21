#include <iostream>
#include <vector>
#include <algorithm>
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
	static string DB_PATH;
	static int readUserDb(vector<User> &users);
	static int registerAccount(vector<User> &users, User user);
};

