#include "infrastructure.h"

using namespace ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {

			std::string exec(const char* cmd) {
				char buffer[128];
				std::string result = "";
				FILE* pipe = popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				try {
					while (!feof(pipe)) {
						if (fgets(buffer, 128, pipe) != NULL)
							result += std::string(buffer);
					}
				}
				catch (...) {
					pclose(pipe);
					throw;
				}
				pclose(pipe);
				return result;
			}

			void set_response(LinuxFileResponse& response, bool success, std::string message) {
				response.Success = success;
				response.ResponseMessage = message;
			}

			LinuxFileException set_exception(std::string errorMessage, OperationType::type type) {
				LinuxFileException linuxFileException;
				linuxFileException.ErrorMessage = errorMessage;
				linuxFileException.Type = type;
				return linuxFileException;
			}

			std::string int_to_string(int64_t integer) {
				std::string result;
				std::ostringstream convert;
				convert << integer;
				result = convert.str();
				return result;
			}

			int set_file_flag(LinuxFileAccess::type fileAccess, LinuxFileMode::type fileMode) {
				int flag = 0;
				switch (fileAccess) {
				case LinuxFileAccess::Read:
					flag |= O_RDONLY;
					break;
				case LinuxFileAccess::Write:
					flag |= O_WRONLY;
					break;
				case LinuxFileAccess::ReadWrite:
				default:
					flag |= O_RDWR;
					break;
				}
				switch (fileMode) {
				case LinuxFileMode::Append:
					flag |= O_APPEND;
					break;
				case LinuxFileMode::OpenOrCreate:
					flag |= O_CREAT;
					break;
				case LinuxFileMode::CreateNew:
					flag |= O_CREAT | O_EXCL;
					break;
				case LinuxFileMode::Truncate:
					flag |= O_TRUNC;
					break;
				// TODO: May not map exactly, consider later
				case LinuxFileMode::Open:
				default:
					break;
				}
				return flag;
			}
		}
	}
}