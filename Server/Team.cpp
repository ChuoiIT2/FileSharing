#include <iostream>
#include "Team.h"

Team::Team() { }

Team::Team(string _name) {
	name = _name;
}

Team::~Team() { }

json Team::toJSON() {
	json j = json{ { "name", name } };
	return j;
}

void Team::fromJSON(const json& j) {
	name = j["name"].get<string>();
}