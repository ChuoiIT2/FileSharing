#pragma once

// Request header code
#define REQ_REGISTER "REGISTER"
#define REQ_LOGIN "LOGIN"
#define REQ_ADDTEAM "ADDTEAM"
#define REQ_JOIN "JOIN"
#define REQ_ACCEPT "ACCEPT"
#define REQ_UPLOAD "UPLOAD"
#define REQ_UPLOADING "UPLOADING"
#define REQ_DOWNLOAD "DOWNLOAD"
#define REQ_DOWNLOADING "DOWNLOADING"
#define REQ_RM "RM"
#define REQ_MKDIR "MKDIR"
#define REQ_RMDIR "RMDIR"
#define REQ_TEAMS "TEAMS"
#define REQ_VIEW "VIEW"

// Response code
#define RES_UNDEFINED_ERROR "000"
#define RES_UNAUTHORIZED_ERROR "001"
#define RES_FORBIDDEN_ERROR "002"

#define RES_REGISTER_SUCCESS "010"
#define RES_REGISTER_ACCOUNT_EXIST "011"

#define RES_LOGIN_SUCCESS "020"
#define RES_LOGIN_WRONG_ACCOUNT "021"

#define RES_ADDTEAM_SUCCESS "030"
#define RES_ADDTEAM_EXISTED "031"

#define RES_JOIN_SUCCESS "040"
#define RES_JOIN_NOT_EXIST "041"
#define RES_JOIN_ALREADY_IN "042"

#define RES_ACCEPT_SUCCESS "050"
#define RES_ACCEPT_NO_REQUEST "051"

#define RES_REQ_UPLOAD_SUCCESS "060"
#define RES_UPLOAD_INVALID_REMOTE_PATH "061"
#define RES_UPLOAD_FILE_EXIST "062"
#define RES_UPLOAD_SUCCESS "063"

#define RES_RM_SUCCESS "070"
#define RES_RM_INVALID_REMOTE_PATH "071"

#define RES_MKDIR_SUCCESS "080"
#define RES_MKDIR_INVALID_PATH "081"
#define RES_MKDIR_DIR_EXIST "082"

#define RES_RMDIR_SUCCESS "090"
#define RES_RMDIR_INVALID_PATH "091"

#define RES_REQ_DOWNLOAD_SUCCESS "100"
#define RES_DOWNLOAD_INVALID_PATH "101"
#define RES_DOWNLOADING "102"

#define RES_TEAMS_SUCCESS "110"

#define RES_VIEW_SUCCESS "120"

// Response message
#define MSG_UNDEFINED_ERROR "Undefined error"
#define MSG_UNAUTHORIZED_ERROR "You have to login to do this action"
#define MSG_FORBIDDEN_ERROR "You don't have permission to do this action"

#define MSG_REGISTER_SUCCESS "Register account successfully"
#define MSG_REGISTER_ACCOUNT_EXIST "This username has already been registered"

#define MSG_LOGIN_SUCCESS "Login successfully"
#define MSG_LOGIN_WRONG_ACCOUNT "Wrong username or password"

#define MSG_ADDTEAM_SUCCESS "Create new team successfully"
#define MSG_ADDTEAM_EXISTED "This name is already used by the other"

#define MSG_JOIN_SUCCESS "Request join team successfully"
#define MSG_JOIN_NOT_EXIST "This team does not exist"
#define MSG_JOIN_ALREADY_IN "You are already in this team"

#define MSG_ACCEPT_SUCCESS "Accept request successfully"
#define MSG_ACCEPT_NO_REQUEST "This request does not exist"

#define MSG_REQ_UPLOAD_SUCCESS "Request upload file successfully"
#define MSG_UPLOAD_SUCCESS "Upload file successfully"
#define MSG_UPLOAD_INVALID_REMOTE_PATH "Invalid remote file path"
#define MSG_UPLOAD_FILE_EXIST "This file path has already existed"
#define MSG_UPLOADING "File is uploading"

#define MSG_RM_SUCCESS "Remove file successfully"
#define MSG_RM_INVALID_REMOTE_PATH "Invalid remote file path"

#define MSG_MKDIR_SUCCESS "Create directory successfully"
#define MSG_MKDIR_INVALID_PATH "Invalid remote parent directory path"
#define MSG_MKDIR_DIR_EXIST "This directory path has already existed"

#define MSG_RMDIR_SUCCESS "Remove directory successfully"
#define MSG_RMDIR_INVALID_PATH "Invalid remote directory path"

#define MSG_REQ_DOWNLOAD_SUCCESS "Request download file successfully"
#define MSG_DOWNLOAD_SUCCESS "Download file successfully"
#define MSG_DOWNLOAD_INVALID_PATH "Invalid remote file path"
#define MSG_DOWNLOADING "File is downloading"

#define MSG_TEAMS_SUCCESS "Get teams successfully"

#define MSG_VIEW_SUCCESS "View directory structure successfully"