#include "User.h"

using namespace std;

User::User() { }

User::User(string _username, string _password) {
	username = _username;
	password = _password;
}

User::~User() { }

int User::readUserDb(string USER_DB_PATH, vector<User> &users) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, USER_DB_PATH.c_str(), "rb");
	if (error != 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return 1;
	}
	char buff[DB_BUFF];
	int readBuff = fread_s(buff, DB_BUFF, 1, DB_BUFF, fUser);
	buff[readBuff] = 0;
	vector<string> lines = Helpers::splitString(string(buff), '\n');
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

int User::registerAccount(string USER_DB_PATH, vector<User> &users, User user) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, USER_DB_PATH.c_str(), "wb");
	if (error != 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return 1;
	} else {
		users.push_back(user);
		string sBuff;
		for (auto _user : users) {
			sBuff += _user.username;
			sBuff += " ";
			sBuff += _user.password;
			sBuff += '\n';
		}
		fwrite(sBuff.c_str(), sBuff.size(), sBuff.size(), fUser);
		fclose(fUser);
		return 0;
	}
}