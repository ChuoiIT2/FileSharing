#pragma once

#include <iostream>
#include <direct.h>
#include <filesystem>
#include "protocols.h"
#include "UserTeam.h"
#include "UserTeamService.h"
#include "Helpers.h"
#include <cstdio>

using namespace std;
using std::experimental::filesystem::recursive_directory_iterator;

class FileService {
public:
	// Store data to a vector of string
	static string viewFileStructure(vector<UserTeam> usersTeams, string teamName, string username, vector<string> &data) {
		if (!UserTeamService::isInTeam(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		string result;

		for (const auto & file : recursive_directory_iterator(ROOT_DATA_PATH + teamName)) {
			data.push_back(file.path().string());
		}

		return RES_VIEW_SUCCESS;
	}

	static string removeFile(vector<UserTeam> usersTeams, string teamName, string username, string data) {
		if (!UserTeamService::isAdmin(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		int result;

		const char* fileName = data.c_str();
		result = remove(fileName);
		if (!result)
			return RES_RM_SUCCESS;
		else
			return RES_RM_INVALID_REMOTE_PATH;
	}
};