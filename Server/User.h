#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "Helpers.h"
#include "protocols.h"


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
};

