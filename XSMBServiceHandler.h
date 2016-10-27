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
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/core.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>

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
					boost::shared_ptr<boost::log::core> core = boost::log::core::get();
					boost::shared_ptr<boost::log::sinks::text_file_backend> backend = 
						boost::make_shared<boost::log::sinks::text_file_backend>
						(
						boost::log::keywords::file_name = "file_%Y-%m-%d_%H-%M-%S.%N.log",
						boost::log::keywords::rotation_size = 10 * 1024 * 1024,
						boost::log::keywords::auto_flush = true
						);
					typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;
					boost::shared_ptr<sink_t> sink(new sink_t(backend));
					sink->set_formatter
						(
						boost::log::expressions::format("[%1%] %2%")
						% boost::log::expressions::attr< boost::posix_time::ptime >("TimeStamp")
						% boost::log::expressions::smessage
						);
					sink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
						boost::log::keywords::target = "logs",
						boost::log::keywords::max_size = 500 * 1024 * 1024
					));
					core->add_sink(sink);
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

				void TruncateFile(LinuxFileResponse& _return, const std::string& filePath);
			private:
				std::map<std::string, std::fstream*> file_handles;
				std::map<int, FILE*> file_pointers;
				boost::log::sources::logger lg;
			};
		}
	}
}