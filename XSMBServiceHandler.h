#include "gen-cpp/XSMBService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			class XSMBServiceHandler : virtual public XSMBServiceIf {
			public:
				XSMBServiceHandler();

				void CreateDirectory(LinuxFileResponse& _return, const std::string& dirPath);

				void DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive);

				void CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering);

				void DeleteFile(LinuxFileResponse& _return, const std::string& filePath);

				void ReadFile(LinuxFileResponse& _return, const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName);

				void WriteFile(LinuxFileResponse& _return, const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName);

				void ListCloudFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive, const std::map<std::string, MatchInformation::type> & files, const std::map<std::string, MatchInformation::type> & dirs);

				int64_t GetCloudFileLength(const std::string& filePath);

				void SetCloudFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength);

				bool XSMBServiceHandler::ListFiles(const std::string& dirPath);

			};
		}
	}
}