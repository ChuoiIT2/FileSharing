#include "Helpers.h"

Helpers::Helpers() { }

Helpers::~Helpers() { }

vector<string> Helpers::splitString(string str, char seperator) {
	vector<string> result;

	int i = 0, len = str.length();
	int startIndex = 0, endIndex = 0;
	while (i <= len) {
		if (str[i] == seperator || i == len) {
			endIndex = i;
			string subStr = "";
			subStr.append(str, startIndex, endIndex - startIndex);
			result.push_back(subStr);
			startIndex = endIndex + 1;
		}
		i++;
	}

	return result;
}

int Helpers::toInt(char* str) {
	char* stopString;
	return strtol(str, &stopString, 10);
}

const wchar_t* Helpers::to_wchar_t(string str) {
	wstring widestr = wstring(str.begin(), str.end());
	return widestr.c_str();
}