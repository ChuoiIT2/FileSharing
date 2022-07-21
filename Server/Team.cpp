#include "Team.h"

Team::Team() { }

Team::Team(string _name) {
	name = _name;
}

Team::~Team() { 
	name = "";
}

string Team::getName() {
	return name;
}

string Team::toString() {
	return name;
}

void Team::setName(string _name) {
	name = _name;
}