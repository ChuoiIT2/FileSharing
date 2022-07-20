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
	if (error == 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return 1;
	}
	fclose(fUser);
	char buff[DB_BUFF];
	int readBuff = fread_s(buff, DB_BUFF, 1, DB_BUFF, fUser);


	return 0;
}

int User::registerAccount(string USER_DB_PATH, vector<User> &users, User user) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, USER_DB_PATH.c_str(), "wb");
	if (error == 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return 1;
	} else {
		users.push_back(user);
	}
}