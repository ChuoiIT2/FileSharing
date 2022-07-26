#include "FileService.h"

/**
* @function viewFileStructure: view file structure of the team
*
* @param userTeams: pointer to the list of user team
* @param teamName: name of the team want to view file structure
* @param username: name of user send request
* @param data: pointer to data list for saving result
* @return RES_FORBIDDEN_ERROR if user not in the team, else RES_VIEW_SUCCESS if success
*/
string FileService::viewFileStructure(vector<UserTeam> usersTeams, string teamName, string username, vector<string> &data) {
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

/**
* @function removeFile: remove file at the directory
*
* @param userTeams: the list of user team
* @param teamName: name of the team
* @param username: name of the user send the request
* @param filePath: remote file path want to remove
* @return RES_FORBIDDEN_ERROR if user not in the team, RES_RM_SUCCESS if remove file success
* RES_RM_INVALID_REMOTE_PATH if remote file not exist
*/
string FileService::removeFile(vector<UserTeam> usersTeams, string teamName, string username, string filePath) {
	if (!UserTeamService::isAdmin(usersTeams, teamName, username)) {
		return RES_FORBIDDEN_ERROR;
	}

	string fullPath = ROOT_DATA_PATH + teamName + "/" + filePath;
	int error = remove(fullPath.c_str());
	if (!error) {
		return RES_RM_SUCCESS;
	}
	else {
		return RES_RM_INVALID_REMOTE_PATH;
	}
}

/**
* @function createDir: create sub directory at the team
*
* @param userTeams: the list of user team
* @param teamName: name of the team
* @param username: name of the user send the request
* @param dirPath: remote directory path
* @param dirName: directory name want to create
* @param RES_FORBIDDEN_ERROR if user not in the team, RES_MKDIR_INVALID_PATH if path hasn't exist,
* RES_MKDIR_DIR_EXIST if directory name has existed, RES_MKDIR_SUCCESS if success, RES_UNDEFINED_ERROR if error is undefined
*/
string FileService::createDir(vector<UserTeam> userTeams, string teamName, string username, string dirPath, string dirName) {
	if (!UserTeamService::isInTeam(userTeams, teamName, username)) {
		return RES_FORBIDDEN_ERROR;
	}

	string rootDirPath = ROOT_DATA_PATH + teamName + "/" + dirPath;

	if (!fs::exists(rootDirPath)) {
		return RES_MKDIR_INVALID_PATH;
	}

	string pathToCreate = rootDirPath + "/" + dirName;
	if (fs::exists(pathToCreate)) {
		return RES_MKDIR_DIR_EXIST;
	}
	if (fs::create_directory(pathToCreate)) {
		return RES_MKDIR_SUCCESS;
	}
	else {
		return RES_UNDEFINED_ERROR;
	}
}

/**
* @function removeDir: remove remote directory at the team
*
* @param userTeams: the list of user team
* @param teamName: name of the team
* @param username: name of the user send the request
* @param dirPath: remove directory path
* @return RES_FORBIDDEN_ERROR if user has no right, RES_RMDIR_INVALID_PATH if remote directory path not exist
* RES_RMDIR_SUCCESS if success
*/
string FileService::removeDir(vector<UserTeam> usersTeams, string teamName, string username, string dirPath) {
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

/**
* @function uploadFile: handle upload file request sent by client
*
* @param userTeams: the list of user team
* @param teamName: name of the team
* @param client: pointer to struct Client type which is saving client information
* @param dirPath: remote directory path want to store file
* @param fileName: name of the file want to upload to server
* @return RES_FORBIDDEN_ERROR if user not in the team, RES_UPLOAD_INVALID_REMOTE_PATH if remote directory path has not exist,
* RES_UPLOAD_FILE_EXIST if file name has exist, RES_REQ_UPLOAD_SUCCESS if success
*/
string FileService::uploadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string dirPath, string fileName) {
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

/**
* @function downloadFile: handle download file request sent by client
*
* @param userTeams: the list of user team
* @param teamName: name of the team
* @param client: pointer to struct Client type which is saving client information
* @param filePath: remote file path want to download
* @return RES_FORBIDDEN_ERROR if user not in the team, RES_DOWNLOAD_INVALID_PATH if file path is invalid,
* RES_REQ_DOWNLOAD_SUCCESS if success
*/
string FileService::downloadFile(vector<UserTeam> usersTeams, string teamName, Client &client, string filePath) {
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