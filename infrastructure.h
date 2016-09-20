#include <gen-cpp/cpp_linux_types.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			void set_response(LinuxFileResponse& response, bool success, std::string message);
			LinuxFileException set_exception(std::string error_message, OperationType::type type);
			std::string int_to_string(int64_t integer);
			int set_file_flag(LinuxFileAccess::type file_access, LinuxFileMode::type file_mode);
		}
	}
}