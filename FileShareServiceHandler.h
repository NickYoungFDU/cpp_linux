#include "gen-cpp/FileShareService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <stdexcept>
#include <iostream>

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
				FileShareServiceHandler();

				void MapFileShare(LinuxFileResponse& _return, const std::string& smbShareAddress, const std::string& username, const std::string& password, const std::string& mountPoint);

				void UnmapFileShare(LinuxFileResponse& _return, const std::string& mountPoint);

				std::string exec(const char* cmd);
			};
		}
	}
}