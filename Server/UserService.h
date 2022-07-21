#pragma once

#include <iostream>
#include "User.h"
#include "protocols.h"

class UserService {
private:
public:
	UserService() { }
	~UserService() { }

	static string DB_PATH;

	static int readDb(vector<User> &users) {
		FILE* fUser;
		errno_t error = fopen_s(&fUser, DB_PATH.c_str(), "rt");
		if (error != 0) {
			cout << "Error: Cannot open USER_DB" << "\n";
			return 1;
		}

		char buff[DB_BUFF];
		vector<User> result;
		while (fgets(buff, DB_BUFF, fUser) != NULL) {
			buff[strlen(buff) - 1] = 0;
			vector<string> lines = Helpers::splitString(string(buff), ' ');
			if (lines.size() != 2) {
				return 1;
			}
			result.push_back(User(lines[0], lines[1]));
		}
		users = result;

		fclose(fUser);
		return 0;
	}

	static string registerAccount(vector<User> &users, User user) {
		FILE* fUser;
		errno_t error = fopen_s(&fUser, DB_PATH.c_str(), "at");
		if (error != 0) {
			cout << "Error: Cannot open USER_DB" << "\n";
			return RES_UNDEFINED_ERROR;
		} else {
			// Check user is already exist
			for (auto _user : users) {
				if (_user.getUsername() == user.getUsername()) {
					return RES_REGISTER_ACCOUNT_EXIST;
				}
			}

			users.push_back(user);
			string line = user.getUsername() + " " + user.getPassword() + "\n";
			fwrite(line.c_str(), sizeof(char), line.length(), fUser);

			fclose(fUser);
			return RES_REGISTER_SUCCESS;
		}
	}

	static string checkLogin(vector<User> users, User user) {
		for (auto _user : users) {
			if (_user.getUsername() == user.getUsername()
				&& _user.getPassword() == user.getPassword()) {
				return RES_LOGIN_SUCCESS;
			}
		}
		return RES_LOGIN_WRONG_ACCOUNT;
	}
};