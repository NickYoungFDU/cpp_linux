#include "gen-cpp/FileShareService.h"
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
#include <boost/log/keywords/format.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
namespace expr = boost::log::expressions;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
using namespace  ::azure::storage::cpp_linux;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux
		{
			class FileShareServiceHandler : virtual public FileShareServiceIf {
			public:
				FileShareServiceHandler(bool logOn) {
					if (logOn) {
						logging::add_file_log(
							boost::log::keywords::file_name = "logs/file_%Y-%m-%d_%H-%M-%S.%N.log",
							boost::log::keywords::rotation_size = 10 * 1024 * 1024,
							boost::log::keywords::auto_flush = true,
							boost::log::keywords::format = "[%TimeStamp%]: %Message%"
							)->locked_backend()->set_file_collector(
							boost::log::sinks::file::make_collector(
							boost::log::keywords::target = "/home/xufyan/cpp_linux/logs",
							boost::log::keywords::max_size = 0x7fffffff
							));
						logging::add_common_attributes();
					}
				}

				void MapFileShare(LinuxFileResponse& _return, const std::string& smbShareAddress, const std::string& username, const std::string& password, const std::string& mountPoint);

				void UnmapFileShare(LinuxFileResponse& _return, const std::string& mountPoint);			

			private:
				boost::log::sources::logger lg;
			};
		}
	}
}