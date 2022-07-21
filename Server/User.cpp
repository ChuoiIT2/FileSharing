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
	int readBuff = fread_s(buff, DB_BUFF, 1, DB_BUFF, fUser);
	buff[readBuff] = 0;
	vector<string> lines = Helpers::splitString(string(buff), '\n');
	users.clear();
	for (auto line : lines) {
		vector<string> rawLine = Helpers::splitString(line, ' ');
		if (rawLine.size() != 2) {
			continue;
		} else {
			users.push_back(User(rawLine[0], rawLine[1]));
		}
	}

	fclose(fUser);
	return 0;
}

int User::registerAccount(vector<User> &users, User user) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, DB_PATH.c_str(), "at");
	if (error != 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return 1;
	} else {
		users.push_back(user);
		string line = user.username + " " + user.password + "\n";
		fwrite(line.c_str(), sizeof(char), line.length(), fUser);

		fclose(fUser);
		return 0;
	}
}

bool User::checkLogin(vector<User> &users, User user) {
	for (auto _user : users) {
		if (_user.username == user.username && _user.password == user.password) {
			return true;
		}
	}
	return false;
}