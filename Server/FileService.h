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

		data.clear();
		for (const auto & file : fs::recursive_directory_iterator(ROOT_DATA_PATH + teamName)) {
			string path = file.path().string().replace(0, 6 + teamName.length(), "");
			data.push_back(path);
		}

		return RES_VIEW_SUCCESS;
	}

	static string removeFile(vector<UserTeam> usersTeams, string teamName, string username, string filePath) {
		if (!UserTeamService::isAdmin(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		string fullPath = ROOT_DATA_PATH + teamName + "/" + filePath;
		int error = remove(fullPath.c_str());
		if (!error) {
			return RES_RM_SUCCESS;
		} else {
			return RES_RM_INVALID_REMOTE_PATH;
		}
	}

	static string createDir(vector<UserTeam> userTeams, string teamName, string username, string dirPath, string dirName) {
		if (!UserTeamService::isInTeam(userTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}

		string rootDirPath = ROOT_DATA_PATH + teamName + "/" + dirPath;

		//Check if directory path has exist
		if (!fs::exists(rootDirPath)) {
			return RES_MKDIR_INVALID_PATH;
		}

		string pathToCreate = rootDirPath + "/" + dirName;
		if (fs::exists(pathToCreate)) {
			return RES_MKDIR_DIR_EXIST;
		}
		if (fs::create_directory(pathToCreate)) {
			return RES_MKDIR_SUCCESS;
		} else {
			return RES_UNDEFINED_ERROR;
		}
	}

	static string removeDir(vector<UserTeam> usersTeams, string teamName, string username, string dirPath) {
		if (!UserTeamService::isAdmin(usersTeams, teamName, username)) {
			return RES_FORBIDDEN_ERROR;
		}
		string pathToRemove = ROOT_DATA_PATH + teamName + "/" + dirPath;
		if (!fs::exists(pathToRemove)) {
			return RES_RMDIR_INVALID_PATH;
		}
		fs::remove_all(fs::path(pathToRemove));

		return RES_RMDIR_SUCCESS;
	}

	static string uploadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string dirPath, string fileName) {
		if (!UserTeamService::isInTeam(usersTeams, teamName, client.username)) {
			return RES_FORBIDDEN_ERROR;
		}
		string fullDirPath = ROOT_DATA_PATH + teamName + "/" + dirPath;
		if (!fs::exists(fullDirPath)) {
			return RES_UPLOAD_INVALID_REMOTE_PATH;
		}
		if (fs::exists(fullDirPath + "/" + fileName)) {
			return RES_UPLOAD_FILE_EXIST;
		}
		client.curTeam = teamName;
		client.curDirPath = dirPath;
		client.curFileName = fileName;

		return RES_REQ_UPLOAD_SUCCESS;
	}

	static string downloadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string filePath) {
		if (!UserTeamService::isInTeam(usersTeams, teamName, client.username)) {
			return RES_FORBIDDEN_ERROR;
		}
		string fullFilePath = ROOT_DATA_PATH + teamName + "/" + filePath;
		if (!fs::exists(fullFilePath)) {
			return RES_DOWNLOAD_INVALID_PATH;
		}

		client.curTeam = teamName;
		client.curFileName = filePath;

		return RES_REQ_DOWNLOAD_SUCCESS;
	}
};