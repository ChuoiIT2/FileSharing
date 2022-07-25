#include "Helpers.h"

Helpers::Helpers() { }

Helpers::~Helpers() { }

/**
* @function splitString: split string by custom seperator
*
* @param str: string for spliting
* @param seperator: seperator
* @return vector store strings which splited
*/
vector<string> Helpers::splitString(string str, char seperator) {
	vector<string> result;

	int i = 0, len = (int) str.length();
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

/**
* @function toInt: convert char* to int
*
* @return int
*/
int Helpers::toInt(char* str) {
	char* stopString;
	return strtol(str, &stopString, 10);
}

/**
* @function to_wchar_t: convert string to wchar_t
* 
* @return wchar_t
*/
const wchar_t* Helpers::to_wchar_t(string str) {
	wstring widestr = wstring(str.begin(), str.end());
	return widestr.c_str();
}

/**
* @functioN printWSAError: print error be sent by WSA
* 
* @param error: WSA error
* @param text: custom error text
* @return void
*/
void Helpers::printWSAError(int error, string text) {
	cout << "Error " << error << ": " << text << endl;
}

/**
* @function convertLength: convert length from int to 4 byte char
* 
* @param length: length type int
* @return 4 byte char 
*/
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

/**
* @function getLength: convert 4 byte char length to int length
* 
* @param cLength: 4 byte char length
* @return int length
**/
const int Helpers::getLength(char *cLength) {
	return (1 << 21) * cLength[0] + (1 << 14) * cLength[1] + (1 << 7) * cLength[2] + cLength[3];
}