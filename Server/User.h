#include <iostream>
#include <vector>
#include <algorithm>
#include "Helpers.h"
#include "protocols.h"

#pragma once

#ifndef DB_BUFF
#define DB_BUFF 200000
#endif

using namespace std;

class User {
private:
	string username;
	string password;

public:
	User();
	User(string username, string password);
	~User();
	string getUsername();
	string getPassword();
	string toString();
	static string DB_PATH;
	static int readDb(vector<User> &users);
	static string registerAccount(vector<User> &users, User user);
	static string checkLogin(vector<User> &users, User user);
};

