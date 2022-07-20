#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Helpers {
public:
	Helpers();
	~Helpers();
	static vector<string> splitString(string str, char seperator);
};

