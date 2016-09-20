/*
 * TODO: Modify namespace properly
 */
namespace csharp Microsoft.Azure.Storage.CppLinux
namespace cpp azure.storage.cpp_linux

enum MatchInformation {
    OnlyOnServer = 1, 
    OnlyInKeys = 2, 
    DeletedOnClient = 3, 
    Match = 4
}

enum LinuxFileMode {
    Append = 0,
    Create = 1, 
    CreateNew = 2, 
    Open = 3, 
    OpenOrCreate = 4, 
    Truncate = 5   
}

enum LinuxFileAccess {
    Read = 0, 
    Write = 1, 
    ReadWrite = 2
}

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
    2: required OperationType Type,
    3: optional map<string, string> AdditionalInfo
}

struct LinuxFileResponse {
    1: required bool Success,
    2: required string ResponseMessage,
    3: required OperationType Type,
    4: optional map<string, string> AdditionalInfo,
    5: optional binary Buffer,
    6: optional map<string, MatchInformation> Directories,
    7: optional map<string, MatchInformation> Files
}

service FileShareService {   
    
    LinuxFileResponse MapFileShare(1:string smbShareAddress, 2: string username, 3:string password, 4:string mountPoint)
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse UnmapFileShare(1:string mountPoint) throws (1:LinuxFileException linuxFileException)
}

service XSMBService {
    
    LinuxFileResponse CreateDirectory(1:string dirPath) throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse DeleteDirectory(1:string dirPath, 2:bool isRecursive) 
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse CreateFile(1:string filePath, 2:i64 fileSize, 3:bool noBuffering)
                        throws (1:LinuxFileException linuxFileException), 
    
    LinuxFileResponse DeleteFile(1:string filePath)
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse ReadFile(1:string filePath, 2:i64 offset, 3:i64 count)
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse WriteFile(1:string filePath, 2:i64 offset, 3:binary buffer, 4:i64 count)
                        throws (1:LinuxFileException linuxFileException),        
    
    LinuxFileResponse ListFiles(1:string dirPath, 2:bool isRecursive)
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse GetFileLength(1:string filePath) throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse SetFileLength(1:string filePath, 2:i64 fileLength) 
                        throws (1:LinuxFileException linuxFileException)
                        
    LinuxFileResponse OpenFileHandle(1:string filePath, 2:LinuxFileMode fileMode, 3:LinuxFileAccess fileAccess, 4:i32 handleId) throws(1:LinuxFileException linuxFileException)
    
    LinuxFileResponse CloseFileHandle(1:i32 handleId) throws(1:LinuxFileException linuxFileException)
    
    LinuxFileResponse ReadFileByHandle(1:i32 handleId, 2:i64 offset, 3:i64 count) throws(1:LinuxFileException linuxFileException)
    
    LinuxFileResponse WriteFileByHandle(1:i32 handleId, 2:i64 offset, 3:binary buffer, 4:i64 count) throws(1:LinuxFileException linuxFileException)                        
}
