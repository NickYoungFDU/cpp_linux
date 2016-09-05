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
				XSMBServiceHandler() {
					// Your initialization goes here
				}

				std::string& GetMountPoint(const std::string& fullPath);

				bool CreateDirectory(const std::string& dirPath);
				bool DeleteDirectory(const std::string& dirPath, const bool isRecursive);
				bool CreateFile(const std::string& filePath, const int64_t fileSize, const bool noBuffering);
				bool DeleteFile(const std::string& filePath);
				bool ReadFile(const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName);
				bool WriteFile(const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName);
				bool ListFiles(const std::string& dirPath);
				bool ListCloudFiles(const std::string& dirPath, const bool isRecursive, const std::map<std::string, MatchInformation::type> & files, const std::map<std::string, MatchInformation::type> & dirs);
				int64_t GetCloudFileLength(const std::string& filePath);
				bool SetCloudFileLength(const std::string& filePath, const int64_t fileLength);

			};
		}
	}
}