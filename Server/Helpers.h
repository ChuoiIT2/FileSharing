#pragma once

#include <iostream>
#include <string>
#include <vector>

#define ROOT_DATA_PATH "data/"

using namespace std;

class Helpers {
public:
	Helpers();
	~Helpers();
	static vector<string> splitString(string str, char seperator);
	static int toInt(char* str);
	static const wchar_t* to_wchar_t(string str);
	static void printWSAError(int error, string text);
	static const char* convertLength(int length);
};