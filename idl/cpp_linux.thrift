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

service FileShareService {   
    
    string MapFileShare(1:string smbShareAddress, 2: string username, 3:string password, 4:string mountPoint),
    
    void UnmapFileContainer(1:string mountPoint)
}

service XSMBService {
    
    bool CreateDirectory(1:string dirPath),
    
    bool DeleteDirectory(1:string dirPath, 2:bool isRecursive),
    
    bool CreateFile(1:string filePath, 2:i64 fileSize, 3:bool noBuffering), 
    
    bool DeleteFile(1:string filePath),
    
    bool ReadFile(1:string filePath, 2:StreamDataLayout data, 3:bool noBuffering, 4:byte fileVersion, 5:bool useVersionInData, 6:string keyName),
    
    bool WriteFile(1:string filePath, 2:StreamDataLayout data, 3:bool noBuffering, 4:byte fileVersion, 5:bool useVersionInData, 6:string keyName),        
    
    bool ListCloudFiles(1:string dirPath, 2:bool isRecursive, 3:map<string, MatchInformation> files, 4:map<string, MatchInformation> dirs),
    
    i64 GetCloudFileLength(1:string filePath),
    
    bool SetCloudFileLength(1:string filePath, 2:i64 fileLength)
}
