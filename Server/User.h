#include <iostream>
#include <vector>

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
	int readUserDb(string USER_DB_PATH, vector<User> &users);
	int registerAccount(string USER_DB_PATH, vector<User> &users, User user);
};

