#include "gen-cpp/XSMBService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			class XSMBServiceHandler : public XSMBServiceIf {
			public:
				XSMBServiceHandler() {

				}

				void CreateDirectory(LinuxFileResponse& _return, const std::string& dirPath);

				void DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive);

				void CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering);

				void DeleteFile(LinuxFileResponse& _return, const std::string& filePath);

				void ReadFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const int64_t count);

				void WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buffer, const int64_t count);

				void ListFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive);

				void GetFileLength(LinuxFileResponse& _return, const std::string& filePath);

				void SetFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength);				

				void OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath, const LinuxFileMode::type fileMode, const LinuxFileAccess::type fileAccess, const int32_t handleId);
				void CloseFileHandle(LinuxFileResponse& _return, const int32_t handleId);
				void ReadFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const int64_t count);
				void WriteFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const std::string& buffer, const int64_t count);

				void PathExists(LinuxFileResponse& _return, const std::string& path);

				void CopyFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists);

				void MoveFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists, const bool fileCopyAllowed);
			private:
				std::map<std::string, std::fstream*> file_handles;
				std::map<int, FILE*> file_pointers;
			};
		}
	}
}