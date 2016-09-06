#include <gen-cpp/cpp_linux_types.h>

using namespace ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			void SetResponse(LinuxFileResponse& response, bool success, std::string message);
			LinuxFileException GetException(std::string errorMessage, OperationType::type type);
		}
	}
}