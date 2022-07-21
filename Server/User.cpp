#include "User.h"

using namespace std;

User::User() { }

User::User(string _username, string _password) {
	if (find(_username.begin(), _username.end(), ' ') != _username.end()
		|| find(_password.begin(), _password.end(), ' ') != _password.end()) {
		cout << "Error: username and password cannot contain any space";
		return;
	}
	username = _username;
	password = _password;
}

User::~User() { }

string User::getUsername() {
	return username;
}

string User::getPassword() {
	return password;
}

int User::readUserDb(vector<User> &users) {
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
		vector<string> lines = Helpers::splitString(string(buff), '\n');
		if (lines.size() != 2) {
			return 1;
		}
		result.push_back(User(lines[0], lines[1]));
	}
	users = result;

	fclose(fUser);
	return 0;
}

string User::registerAccount(vector<User> &users, User user) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, DB_PATH.c_str(), "at");
	if (error != 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return RES_UNDEFINED_ERROR;
	} else {
		// Check user is already exist
		for (auto _user : users) {
			if (_user.username == user.username) {
				return RES_REGISTER_ACCOUNT_EXIST;
			}
		}

		users.push_back(user);
		string line = user.username + " " + user.password + "\n";
		fwrite(line.c_str(), sizeof(char), line.length(), fUser);

		fclose(fUser);
		return RES_REGISTER_SUCCESS;
	}
}

string User::checkLogin(vector<User> &users, User user) {
	for (auto _user : users) {
		if (_user.username == user.username && _user.password == user.password) {
			return RES_LOGIN_SUCCESS;
		}
	}
	return RES_LOGIN_WRONG_ACCOUNT;
}