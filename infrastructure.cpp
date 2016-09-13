#include "infrastructure.h"

using namespace ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			void SetResponse(LinuxFileResponse& response, bool success, std::string message) {
				response.Success = success;
				response.ResponseMessage = message;
			}

			LinuxFileException GetException(std::string errorMessage, OperationType::type type) {
				LinuxFileException linuxFileException;
				linuxFileException.ErrorMessage = errorMessage;
				linuxFileException.Type = type;
				return linuxFileException;
			}

			std::string IntToString(int64_t integer) {
				std::string result;
				std::ostringstream convert;
				convert << integer;
				result = convert.str();
				return result;
			}
		}
	}
}