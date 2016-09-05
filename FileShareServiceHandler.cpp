#include <FileShareServiceHandler.h>

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux
		{
			void FileShareServiceHandler::MapFileShare(std::string& _return, const std::string& smbShareAddress, const std::string& username, const std::string& password, const std::string& mountPoint) {
				// Your implementation goes here
				std::string xsmbVersion = "2.1";
				std::string mountCommand = "mount -t cifs " + smbShareAddress + " " + mountPoint + " -o vers='" + xsmbVersion + "',username='" + username + "',password='" + password + "',dir_mode=0777,file_mode=0777";
				std::string ret = exec(mountCommand.c_str());
				std::cout << ret << std::endl;
			}

			void FileShareServiceHandler::UnmapFileContainer(const std::string& mountPoint) {
				// Your implementation goes here
				std::string unmountCommand = "umount " + mountPoint;
				std::string ret = exec(unmountCommand.c_str());
				std::cout << ret << std::endl;

			}

						
			std::string FileShareServiceHandler::exec(const char* cmd) {
				char buffer[128];
				std::string result = "";
				FILE* pipe = popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				try {
					while (!feof(pipe)) {
						if (fgets(buffer, 128, pipe) != NULL)
							result += buffer;
					}
				}
				catch (...) {
					pclose(pipe);
					throw;
				}
				pclose(pipe);
				return result;
			}
			
		}
	}
}
