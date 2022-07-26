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
	static string viewFileStructure(vector<UserTeam> usersTeams, string teamName, string username, vector<string> &data);
	static string removeFile(vector<UserTeam> usersTeams, string teamName, string username, string filePath);
	static string createDir(vector<UserTeam> userTeams, string teamName, string username, string dirPath, string dirName);
	static string removeDir(vector<UserTeam> usersTeams, string teamName, string username, string dirPath);
	static string uploadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string dirPath, string fileName);
	static string downloadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string filePath);
};