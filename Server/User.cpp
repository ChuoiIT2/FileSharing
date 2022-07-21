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

string User::toString() {
	return username + " " + password;
}
