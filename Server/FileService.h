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
namespace fs = std::experimental::filesystem;

class FileService {
public:
	// Store data to a vector of string
	static string viewFileStructure(vector<UserTeam> usersTeams, string teamName, string username, vector<string> &data) {
		if (!UserTeamService::isInTeam(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		string result;

		for (const auto & file : fs::recursive_directory_iterator(ROOT_DATA_PATH + teamName)) {
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

	static string createDir(vector<UserTeam> userTeams, string teamName, string username, string dirPath, string dirName) {
		if (!UserTeamService::isInTeam(userTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		string rootDirPath = ROOT_DATA_PATH + teamName + "/" + dirPath;

		if (fs::exists(rootDirPath)) {
			return RES_MKDIR_INVALID_PATH;
		}

		string pathToCreate = dirPath + "/" + dirName;
		if (fs::exists(pathToCreate)) {
			return RES_MKDIR_DIR_EXIST;
		}
		return RES_MKDIR_SUCCESS;
	}

	static string removeDir(vector<UserTeam> usersTeams, string teamName, string username, string dirPath) {
		if (!UserTeamService::isAdmin(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}
		string pathToRemove = ROOT_DATA_PATH + teamName + "/" + dirPath;
		auto numRemoved = fs::remove_all(fs::path(pathToRemove));
		if (numRemoved == 0) {
			return RES_RMDIR_INVALID_PATH;
		}
		return RES_RMDIR_SUCCESS;
	}
};