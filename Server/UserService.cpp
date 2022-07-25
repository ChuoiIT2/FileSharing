#include "UserService.h"

/**
* Default contructor of UserService
*/
UserService::UserService() {

}

/**
* Destructors of UserService
*/
UserService::~UserService() {

}

/**
* @function readDb: read team data and store it to the current user list
*
* @param teams: pointer to the current user list
* @return 0 if success, 1 if error
*/
int UserService::readDb(vector<User> &users) {
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

/**
* @function registerAccount: handle request register account from user
* 
* @param users: pointer to the current user list
* @param user: store user information to register
* @return RES_REGISTER_ACCOUNT_EXIST if user has been existed, RES_REGISTER_SUCCESS if register success
* RES_UNDEFINED_ERROR if error is undefined
*/
string UserService::registerAccount(vector<User> &users, User user) {
	FILE* fUser;
	errno_t error = fopen_s(&fUser, DB_PATH.c_str(), "at");
	if (error != 0) {
		cout << "Error: Cannot open USER_DB" << "\n";
		return RES_UNDEFINED_ERROR;
	}
	else {
		for (auto _user : users) {
			if (_user.getUsername() == user.getUsername()) {
				fclose(fUser);
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

/**
* @function checkLogin: check if login of user
* 
* @param users: pointer to current users list
* @param user: store user information for checking login
* @return RES_LOGIN_SUCCESS if login success, RES_LOGIN_WRONG_ACCOUNT if username/password wrong
*/
string UserService::checkLogin(vector<User> users, User user) {
	for (auto _user : users) {
		if (_user.getUsername() == user.getUsername()
			&& _user.getPassword() == user.getPassword()) {
			return RES_LOGIN_SUCCESS;
		}
	}
	return RES_LOGIN_WRONG_ACCOUNT;
}