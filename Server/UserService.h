#pragma once

#include <iostream>
#include "User.h"
#include "protocols.h"

class UserService {
private:
public:
	UserService();
	~UserService();

	static string DB_PATH;

	static int readDb(vector<User> &users);
	static string registerAccount(vector<User> &users, User user);
	static string checkLogin(vector<User> users, User user);
};