namespace csharp Microsoft.Azure.Storage.CppLinux
namespace cpp azure.storage.cpp_linux

enum OperationType {
    MapFileShare = 0,
    UnmapFileShare = 1, 
    CreateDirectory = 2, 
    DeleteDirectory = 3, 
    CreateFile = 4, 
    DeleteFile = 5, 
    ReadFile = 6, 
    WriteFile = 7,
    ListFile = 8,
    GetFileLength = 9,
    SetFileLength = 10
}

exception LinuxFileException {
    1: required string ErrorMessage,
    2: required OperationType OperationType,
    3: optional map<string, string> AdditionalInfo
}

struct LinuxFileResponse {
    1: required bool Success,
    2: required string ResponseMessage,
    3: required OperationType OperationType,
    4: optional map<string, string> AdditionalInfo
}