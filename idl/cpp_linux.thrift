/*
 * TODO: Modify namespace properly
 */
namespace csharp Microsoft.Azure.Storage.CppLinux
namespace cpp azure.storage.cpp_linux

struct ChunkInfo {
    1:i64   OffSet,
    2:i32   Length,
    3:byte  Version,
    4:bool  IsNullDataWritten,
    5:bool  IsCorrupted
}

struct StreamDataLayout {
    1:list<ChunkInfo> Chunks,
    2:i64 Length    
}

enum MatchInformation {
    OnlyOnServer = 1, 
    OnlyInKeys = 2, 
    DeletedOnClient = 3, 
    Match = 4
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
    4: optional map<string, string> AdditionalInfo
}

struct GetFileLengthResponse {
    1: required i64 FileLength,
    2: required bool Success, 
    3: optional string ErrorMessage
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
    
    LinuxFileResponse ReadFile(1:string filePath, 2:StreamDataLayout data, 3:bool noBuffering, 4:byte fileVersion, 5:bool useVersionInData, 6:string keyName)
                        throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse WriteFile(1:string filePath, 2:StreamDataLayout data, 3:bool noBuffering, 4:byte fileVersion, 5:bool useVersionInData, 6:string keyName)
                        throws (1:LinuxFileException linuxFileException),        
    
    LinuxFileResponse ListFiles(1:string dirPath, 2:bool isRecursive, 3:map<string, MatchInformation> files, 4:map<string, MatchInformation> dirs)
                        throws (1:LinuxFileException linuxFileException),
    
    GetFileLengthResponse GetFileLength(1:string filePath) throws (1:LinuxFileException linuxFileException),
    
    LinuxFileResponse SetFileLength(1:string filePath, 2:i64 fileLength) 
                        throws (1:LinuxFileException linuxFileException)
}
