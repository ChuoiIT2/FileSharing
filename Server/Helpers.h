#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Helpers {
public:
	Helpers();
	~Helpers();
	static vector<string> splitString(string str, char seperator);
	static int toInt(char* str);
	static const wchar_t* to_wchar_t(string str);
};

