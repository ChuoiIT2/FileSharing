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

void Helpers::printWSAError(int error, string text) {
	cout << "Error " << error << ": " << text << endl;
}

const char* Helpers::convertLength(int length) {
	char* result = new char[4];
	result[0] = length / 2097152;
	length %= 2097152;
	result[1] = length / 16384;
	length %= 16384;
	result[2] = length / 128;
	length %= 128;
	result[3] = length;

	return result;
}